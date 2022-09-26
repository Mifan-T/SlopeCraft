/*
 Copyright © 2021-2022  TokiNoBug
This file is part of SlopeCraft.

    SlopeCraft is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SlopeCraft is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SlopeCraft.  If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/ToKiNoBug
    bilibili:https://space.bilibili.com/351429231
*/

#include "TokiSlopeCraft.h"
#include <zlib.h>

const Eigen::Array<float, 2, 3> TokiSlopeCraft::DitherMapLR = {
    {0.0 / 16.0, 0.0 / 16.0, 7.0 / 16.0}, {3.0 / 16.0, 5.0 / 16.0, 1.0 / 16.0}};
const Eigen::Array<float, 2, 3> TokiSlopeCraft::DitherMapRL = {
    {7.0 / 16.0, 0.0 / 16.0, 0.0 / 16.0}, {1.0 / 16.0, 5.0 / 16.0, 3.0 / 16.0}};

const colorset_basic_t TokiSlopeCraft::Basic(SlopeCraft::RGBBasicSource);
colorset_allowed_t TokiSlopeCraft::Allowed;

gameVersion TokiSlopeCraft::mcVer; // 12,13,14,15,16,17
mapTypes TokiSlopeCraft::mapType;
std::vector<simpleBlock> TokiSlopeCraft::blockPalette(0);

std::unordered_set<TokiSlopeCraft *> TokiSlopeCraft::kernel_hash_set;

TokiSlopeCraft::TokiSlopeCraft() {
  kernelStep = step::nothing;
  rawImage.setZero(0, 0);

  glassBuilder = new PrimGlassBuilder;
  Compressor = new LossyCompressor;
  GAConverter = new GACvter::GAConverter;
  setProgressRangeSet([](void *, int, int, int) {});
  setProgressAdd([](void *, int) {});
  setKeepAwake([](void *) {});
  setReportError([](void *, errorFlag, const char *) {});
  setReportWorkingStatue([](void *, workStatues) {});
  setAlgoProgressAdd([](void *, int) {});
  setAlgoProgressRangeSet([](void *, int, int, int) {});

  glassBuilder->windPtr = &wind;
  glassBuilder->progressAddPtr = &this->algoProgressAdd;
  glassBuilder->progressRangeSetPtr = &this->algoProgressRangeSet;
  glassBuilder->keepAwakePtr = &this->keepAwake;

  Compressor->windPtr = &wind;
  Compressor->progressAddPtr = &this->algoProgressAdd;
  Compressor->progressRangeSetPtr = &this->algoProgressRangeSet;
  Compressor->keepAwakePtr = &this->keepAwake;

  TokiSlopeCraft::kernel_hash_set.emplace(this);
}

TokiSlopeCraft::~TokiSlopeCraft() {
  delete Compressor;
  delete glassBuilder;
  delete GAConverter;

  TokiSlopeCraft::kernel_hash_set.erase(this);
}

/// function ptr to window object
void TokiSlopeCraft::setWindPtr(void *_w) {
  wind = _w;
  GAConverter->setUiPtr(_w);
}
/// a function ptr to show progress of converting and exporting
void TokiSlopeCraft::setProgressRangeSet(void (*prs)(void *, int, int, int)) {
  progressRangeSet = prs;
  GAConverter->setProgressRangeFun(prs);
}
/// a function ptr to add progress value
void TokiSlopeCraft::setProgressAdd(void (*pa)(void *, int)) {
  progressAdd = pa;
  GAConverter->setProgressAddFun(pa);
}
/// a function ptr to prevent window from being syncoped
void TokiSlopeCraft::setKeepAwake(void (*ka)(void *)) { keepAwake = ka; }

/// a function ptr to show progress of compressing and bridge-building
void TokiSlopeCraft::setAlgoProgressRangeSet(void (*aprs)(void *, int, int,
                                                          int)) {
  algoProgressRangeSet = aprs;
}
/// a function ptr to add progress value of compressing and bridge-building
void TokiSlopeCraft::setAlgoProgressAdd(void (*apa)(void *, int)) {
  algoProgressAdd = apa;
}

/// a function ptr to report error when something wrong happens
void TokiSlopeCraft::setReportError(void (*re)(void *, errorFlag,
                                               const char *)) {
  reportError = re;
}
/// a function ptr to report working statue especially when busy
void TokiSlopeCraft::setReportWorkingStatue(void (*rws)(void *, workStatues)) {
  reportWorkingStatue = rws;
}

void TokiSlopeCraft::decreaseStep(step _step) {
  if (kernelStep <= _step)
    return;
  kernelStep = _step;
}

void TokiSlopeCraft::trySkipStep(step s) {
  if (this->kernelStep >= s) {
    return;
  }

  if (Allowed.color_count() != 0 && blockPalette.size() != 0) {
    this->kernelStep = step::wait4Image;
  }
}

bool compressFile(const char *inputPath, const char *outputPath) {
  const size_t BUFFER_SIZE = 4096;
  std::ifstream fin;
  fin.open(inputPath, std::ifstream::binary | std::ifstream::in);
  if (!fin)
    return false;
  std::vector<char> buffer(BUFFER_SIZE, 0);
  gzFile fout = gzopen(outputPath, "wb");
  while (!fin.eof()) {
    fin.read(buffer.data(), buffer.size());
    std::streamsize s = fin.gcount();
    gzwrite(fout, buffer.data(), s);
  }
  gzclose(fout);
  return true;
}

step TokiSlopeCraft::queryStep() const { return kernelStep; }

void TokiSlopeCraft::setAiCvterOpt(const AiCvterOpt *_a) { AiOpt = *_a; }

const AiCvterOpt *TokiSlopeCraft::aiCvterOpt() const { return &AiOpt; }

bool TokiSlopeCraft::setType(mapTypes type, gameVersion ver,
                             const bool *allowedBaseColor,
                             const AbstractBlock **palettes) {
  static std::mutex lock;
  lock.lock();
  /*
  if (kernelStep < colorSetReady)
  {
      reportError(wind, errorFlag::HASTY_MANIPULATION,
                  "You must load colorset before you set the map type.");
      return false;
  }
  */

  TokiSlopeCraft::mapType = type;
  TokiSlopeCraft::mcVer = ver;

  TokiColor::needFindSide = (TokiSlopeCraft::mapType == mapTypes::Slope);

  TokiSlopeCraft::blockPalette.resize(64);

  // cerr<<__FILE__<<__LINE__<<endl;
  for (short i = 0; i < 64; i++) {
    // cerr<<"Block_"<<i<<endl;
    if (palettes[i] == nullptr) {
      TokiSlopeCraft::blockPalette[i].clear();
      continue;
    }
    palettes[i]->copyTo(&TokiSlopeCraft::blockPalette[i]);

    if (TokiSlopeCraft::blockPalette[i].id.find(':') ==
        TokiSlopeCraft::blockPalette[i].id.npos) {
      TokiSlopeCraft::blockPalette[i].id =
          "minecraft:" + TokiSlopeCraft::blockPalette[i].id;
    }

    if (TokiSlopeCraft::blockPalette[i].idOld.empty()) {
      TokiSlopeCraft::blockPalette[i].idOld = blockPalette[i].id;
    }

    if (TokiSlopeCraft::blockPalette[i].idOld.size() > 0 &&
        (TokiSlopeCraft::blockPalette[i].idOld.find(':') ==
         TokiSlopeCraft::blockPalette[i].idOld.npos)) {
      TokiSlopeCraft::blockPalette[i].idOld =
          "minecraft:" + TokiSlopeCraft::blockPalette[i].idOld;
    }
  }

  // cerr<<__FILE__<<__LINE__<<endl;

  reportWorkingStatue(wind, workStatues::collectingColors);

  Eigen::ArrayXi baseColorVer(64); //基色对应的版本
  baseColorVer.setConstant(FUTURE);
  baseColorVer.segment(0, 52).setConstant(ANCIENT);
  baseColorVer.segment(52, 7).setConstant(MC16);
  baseColorVer.segment(59, 3).setConstant(MC17);

  bool MIndex[256];

  for (short index = 0; index < 256; index++) {
    MIndex[index] = true; //默认可以使用这种颜色，逐次判断各个不可以使用的条件

    if (!allowedBaseColor[index2baseColor(
            index)]) { //在allowedBaseColor中被禁止
      MIndex[index] = false;
      continue;
    }
    if (index2baseColor(index) == 0) { //空气禁用
      MIndex[index] = false;
      continue;
    }
    if (mcVer < baseColorVer(index2baseColor(index))) { //版本低于基色版本
      MIndex[index] = false;
      continue;
    }
    if (TokiSlopeCraft::blockPalette[index2baseColor(index)]
            .id.empty()) { //空id
      MIndex[index] = false;
      continue;
    }
    /*
    if ((mapType == mapTypes::Wall) &&
        !blockPalette[index2baseColor(index)]
             .wallUseable) { //墙面像素画且当前方块不适合墙面

      MIndex[index] = false;
      continue;
    }*/
    if (isVanilla() &&
        (index2depth(index) >= 3)) { //可实装的地图画不允许第四种阴影
      MIndex[index] = false;
      continue;
    }
    if (index2baseColor(index) == 12 &&
        TokiSlopeCraft::mapType != mapTypes::Wall) { //如果是水且非墙面
      if (isFlat() && index2depth(index) != 2) {     //平板且水深不是1格
        MIndex[index] = false;
        continue;
      }
    } else {
      if (isFlat() && index2depth(index) != 1) { //平板且阴影不为1
        MIndex[index] = false;
        continue;
      }
    }
  }

  if (!TokiSlopeCraft::Allowed.apply_allowed(Basic, MIndex)) {
    std::string msg = "Too few usable color(s) : only " +
                      std::to_string(Allowed.color_count()) + " colors\n";
    msg += "Avaliable base color(s) : ";

    for (int idx = 0; idx < Allowed.color_count(); idx++) {
      msg += std::to_string(Allowed.Map(idx)) + ", ";
    }

    reportError(wind, errorFlag::USEABLE_COLOR_TOO_FEW, msg.data());
    lock.unlock();
    return false;
  }

  reportWorkingStatue(wind, workStatues::none);
  for (auto kernel_ptr : TokiSlopeCraft::kernel_hash_set) {
    kernel_ptr->kernelStep = wait4Image;
  }

  lock.unlock();
  return true;
}

bool TokiSlopeCraft::setType(mapTypes type, gameVersion ver,
                             const bool *allowedBaseColor,
                             const simpleBlock *palettes) {
  const AbstractBlock *temp[64];
  for (uint16_t idx = 0; idx < 64; idx++) {
    temp[idx] = palettes + idx;
  }

  return setType(type, ver, allowedBaseColor, temp);
}

void TokiSlopeCraft::configGAConverter() {
  GACvter::updateMapColor2GrayLUT();

  GAConverter->setRawImage(rawImage);
}

uint16_t TokiSlopeCraft::getColorCount() const {
  if (kernelStep < wait4Image) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only query for avaliable color count after you set "
                "the map type and gameversion");
    return 0;
  }
  return Allowed.color_count();
}

void TokiSlopeCraft::getAvailableColors(ARGB *const ARGBDest,
                                        uint8_t *const mapColorDest,
                                        int *const num) const {
  if (num != nullptr) {
    *num = getColorCount();
  }

  for (int idx = 0; idx < TokiSlopeCraft::Allowed.color_count(); idx++) {

    if (mapColorDest != nullptr) {
      mapColorDest[idx] = Allowed.Map(idx);
    }

    if (ARGBDest != nullptr) {
      ARGB r, g, b, a;
      if (mapColor2baseColor(Allowed.Map(idx)) != 0)
        a = 255;
      else
        a = 0;

      r = ARGB(Allowed.RGB(idx, 0) * 255);
      g = ARGB(Allowed.RGB(idx, 1) * 255);
      b = ARGB(Allowed.RGB(idx, 2) * 255);

      ARGBDest[idx] = (a << 24) | (r << 16) | (g << 8) | (b);
    }
  }
}

void TokiSlopeCraft::setRawImage(const ARGB *src, int rows, int cols) {
  setRawImage(EImage::Map(src, rows, cols));
}

void TokiSlopeCraft::setRawImage(const EImage &_rawimg) {
  if (kernelStep < wait4Image) {
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can only import the raw image count after you set the map "
                "type and gameversion");
    return;
  }
  if (_rawimg.size() <= 0) {
    reportError(wind, errorFlag::EMPTY_RAW_IMAGE,
                "The size of your raw image is 0. You loaded an empty image.");
    return;
  }

  rawImage = _rawimg;
  kernelStep = convertionReady;
  configGAConverter();
  return;
}

bool TokiSlopeCraft::isVanilla() const { return mapType != FileOnly; }

bool TokiSlopeCraft::isFlat() const {
  return mapType == Flat || mapType == Wall;
}

void TokiSlopeCraft::getBaseColorInARGB32(ARGB *const dest) const {
  if (dest == nullptr)
    return;

  for (uchar base = 0; base < 64; base++)
    dest[base] =
        ARGB32(255 * Basic.RGB(128 + base, 0), 255 * Basic.RGB(128 + base, 1),
               255 * Basic.RGB(128 + base, 2), 255);
}

int64_t TokiSlopeCraft::sizePic(short dim) const {
  if (dim == 0)
    return rawImage.rows();
  if (dim == 1)
    return rawImage.cols();
  return rawImage.size();
}

void TokiSlopeCraft::getTokiColorPtr(uint16_t col,
                                     const TokiColor **dst) const {
  if (kernelStep < converted) {
    cerr << "Too hasty! export after you converted the map!" << endl;
    reportError(wind, errorFlag::HASTY_MANIPULATION,
                "You can export only after you converted a map.");
    return;
  }
  for (uint16_t r = 0; r < ditheredImage.rows(); r++) {
    auto i = colorHash.find(ditheredImage(r, col));

    if (i == colorHash.end())
      dst[r] = nullptr;
    else
      dst[r] = (const TokiColor *)&(colorHash.at(ditheredImage(r, col)));
  }
}

TokiSlopeCraft::ColorSpace TokiSlopeCraft::getColorSpace() const {
  switch (ConvertAlgo) {
  case RGB:
    return R;
  case RGB_Better:
    return R;
  case HSV:
    return H;
  case Lab94:
    return L;
  case convertAlgo::Lab00:
    return L;
  case XYZ:
    return X;
  case convertAlgo::gaCvter:
    return R;
  }
  return R;
}

void TokiSlopeCraft::getConvertedImage(int *rows, int *cols, ARGB *dest) const {
  EImage result = getConovertedImage();
  if (rows != nullptr)
    *rows = result.rows();
  if (cols != nullptr)
    *cols = result.cols();
  if (dest != nullptr)
    memcpy(dest, result.data(), sizeof(ARGB) * sizePic(2));
}

EImage TokiSlopeCraft::getConovertedImage() const {
  EImage cvtedImg(sizePic(0), sizePic(1));
  cvtedImg.setZero();
  if (kernelStep < converted) {
    reportError(
        wind, errorFlag::HASTY_MANIPULATION,
        "You can get the converted image only after you converted a map.");
    return cvtedImg;
  }

  Eigen::Array<ARGB, 256, 1> argbLUT;
  for (int idx = 0; idx < 256; idx++) {
    argbLUT[idx] =
        RGB2ARGB(Basic.RGB(idx, 0), Basic.RGB(idx, 1), Basic.RGB(idx, 2));
  }

  // RGBint =(RGBint > 255).select(Eigen::ArrayXXi::Constant(256, 3, 255),
  // RGBint);
  // short Index;
  for (short r = 0; r < sizePic(0); r++) {
    for (short c = 0; c < sizePic(1); c++) {
      if (mapColor2baseColor(this->mapPic(r, c)) == 0) { //  if base ==0
        cvtedImg(r, c) = ARGB32(0, 0, 0, 0);
        continue;
      }
      const int Index = mapColor2Index(this->mapPic(r, c));

      cvtedImg(r, c) = argbLUT[Index];
    }
  }
  return cvtedImg;
}

void TokiSlopeCraft::getConvertedMap(int *rows, int *cols,
                                     unsigned char *dst) const {
  if (rows != nullptr) {
    *rows = getImageRows();
  }
  if (cols != nullptr) {
    *cols = getImageCols();
  }

  Eigen::Map<Eigen::Array<uint8_t, Eigen::Dynamic, Eigen::Dynamic>> dest(
      dst, getImageRows(), getImageCols());

  for (int r = 0; r < getImageRows(); r++) {
    for (int c = 0; c < getImageCols(); c++) {
      dest(r, c) = colorHash.find(ditheredImage(r, c))->second.Result;
    }
  }
}

int TokiSlopeCraft::getImageRows() const {
  if (kernelStep < convertionReady) {
    reportError(
        wind, errorFlag::HASTY_MANIPULATION,
        "You can call getImageRows only after you imported the raw image.");
    return -1;
  }
  return rawImage.rows();
}

int TokiSlopeCraft::getImageCols() const {
  if (kernelStep < convertionReady) {
    reportError(
        wind, errorFlag::HASTY_MANIPULATION,
        "You can call getImageRows only after you imported the raw image.");
    return -1;
  }
  return rawImage.cols();
}

void TokiSlopeCraft::get3DSize(int *x, int *y, int *z) const {
  if (kernelStep < builded)
    return;
  if (x != nullptr)
    *x = schem.x_range();
  if (y != nullptr)
    *y = schem.y_range();
  if (z != nullptr)
    *z = schem.z_range();
  return;
}

int TokiSlopeCraft::getHeight() const {
  if (kernelStep < builded)
    return -1;
  return schem.y_range();
}

void TokiSlopeCraft::getBlockCounts(int *total, int detail[64]) const {
  std::vector<int64_t> temp;
  if (total != nullptr)
    *total = getBlockCounts(&temp);
  if (detail != nullptr)
    for (uint16_t idx = 0; idx < temp.size(); idx++) {
      detail[idx] = temp[idx];
    }
}

int64_t TokiSlopeCraft::getBlockCounts(std::vector<int64_t> *dest) const {
  if (dest == nullptr) {
    return -1;
  }
  if (kernelStep < builded)
    return -1;
  dest->resize(64);
  for (int i = 0; i < 64; i++)
    (*dest)[i] = 0;
  for (int i = 0; i < schem.size(); i++) {
    if (schem(i))
      (*dest)[schem(i) - 1]++;
  }
  int totalBlockCount = 0;
  for (int i = 0; i < 64; i++)
    totalBlockCount += (*dest)[i];
  return totalBlockCount;
}

int64_t TokiSlopeCraft::getBlockCounts() const {
  if (kernelStep < builded)
    return -1;
  int totalCount = 0;
  for (int i = 0; i < schem.size(); i++) {
    if (schem(i))
      totalCount++;
  }
  return totalCount;
}

const uint16_t *TokiSlopeCraft::getBuild(int *xSize, int *ySize,
                                         int *zSize) const {
  if (xSize != nullptr)
    *xSize = getXRange();
  if (ySize != nullptr)
    *ySize = getHeight();
  if (zSize != nullptr)
    *zSize = getZRange();
  return schem.data();
}

int TokiSlopeCraft::getXRange() const {
  if (kernelStep < builded)
    return -1;
  return schem.x_range();
}
int TokiSlopeCraft::getZRange() const {
  if (kernelStep < builded)
    return -1;
  return schem.x_range();
}
