/*
 Copyright © 2021-2023  TokiNoBug
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
    along with SlopeCraft. If not, see <https://www.gnu.org/licenses/>.

    Contact with me:
    github:https://github.com/SlopeCraft/SlopeCraft
    bilibili:https://space.bilibili.com/351429231
*/

#ifndef KERNEL_H
#define KERNEL_H
#include <stddef.h>
#include <stdint.h>

#include "SlopeCraftL_global.h"

// define enumerations first
#ifdef SCL_FIND_GlobalEnums_BY_PATH
#include "../utilities/SC_GlobalEnums.h"
#else
#include "SC_GlobalEnums.h"
#endif

// declare classes
namespace SlopeCraft {
class AbstractBlock;
class Kernel;
struct AiCvterOpt;
}  // namespace SlopeCraft

namespace SlopeCraft {
using step = ::SCL_step;
using mapTypes = ::SCL_mapTypes;
using compressSettings = ::SCL_compressSettings;
using convertAlgo = ::SCL_convertAlgo;
using glassBridgeSettings = ::SCL_glassBridgeSettings;
using gameVersion = ::SCL_gameVersion;
using workStatues = ::SCL_workStatues;
using errorFlag = ::SCL_errorFlag;

}  // namespace SlopeCraft

namespace SlopeCraft {

struct AiCvterOpt {
  uint64_t version{SC_VERSION_U64};
  size_t popSize{50};
  size_t maxGeneration{200};
  size_t maxFailTimes{50};
  double crossoverProb{0.8};
  double mutationProb{0.01};
};

struct StringDeliver {
  StringDeliver() = default;
  StringDeliver(char *p, size_t cap) : data(p), capacity(cap) {}
  char *const data{nullptr};
  const size_t capacity{0};
  size_t size{0};
  bool is_complete{true};

  constexpr bool is_valid() const noexcept {
    return this->data != nullptr && this->capacity > 0;
  }

  template <class string_t>
  [[nodiscard]] static StringDeliver from_string(string_t &s) noexcept {
    return StringDeliver{s.data(), s.size()};
  }
};

class AbstractBlock {
 public:
  AbstractBlock() = default;
  virtual ~AbstractBlock() = default;

  /// id of this block
  virtual const char *getId() const noexcept = 0;
  /// first version
  virtual uint8_t getVersion() const noexcept = 0;
  /// id in 1.12
  virtual const char *getIdOld() const noexcept = 0;
  /// if this block needs a glass block under it
  virtual bool getNeedGlass() const noexcept = 0;
  /// if this block emits light
  virtual bool getDoGlow() const noexcept = 0;
  /// if this block can be stolen by enderman
  virtual bool getEndermanPickable() const noexcept = 0;
  /// if this block can be burnt
  virtual bool getBurnable() const noexcept = 0;

  virtual const char *getNameZH() const noexcept = 0;
  virtual const char *getNameEN() const noexcept = 0;
  virtual const char *getImageFilename() const noexcept = 0;

  constexpr int imageRows() const noexcept { return 16; }
  constexpr int imageCols() const noexcept { return 16; }

  virtual void getImage(uint32_t *dest_row_major) const noexcept = 0;

  /// set block id
  virtual void setId(const char *) noexcept = 0;
  /// set first version
  virtual void setVersion(uint8_t) noexcept = 0;
  /// set id in 1.12
  virtual void setIdOld(const char *) noexcept = 0;
  /// set if this block needs a glass block under it
  virtual void setNeedGlass(bool) noexcept = 0;
  /// set if this block emits light
  virtual void setDoGlow(bool) noexcept = 0;
  /// set if this block can be stolen by enderman
  virtual void setEndermanPickable(bool) noexcept = 0;
  /// set if this block can be burnt
  virtual void setBurnable(bool) noexcept = 0;

  virtual void setNameZH(const char *) noexcept = 0;
  virtual void setNameEN(const char *) noexcept = 0;
  virtual void setImageFilename(const char *) noexcept = 0;

  virtual void setImage(const uint32_t *src, bool is_row_major) noexcept = 0;

  /// let *b equal to *this
  virtual void copyTo(AbstractBlock *b) const noexcept = 0;
  /// set this block to air
  virtual void clear() noexcept;

  virtual const char *idForVersion(SCL_gameVersion ver) const noexcept = 0;
};

class BlockListInterface {
 public:
  BlockListInterface() = default;
  virtual ~BlockListInterface() = default;
  virtual size_t size() const noexcept = 0;
  virtual size_t get_blocks(AbstractBlock **, uint8_t *,
                            size_t capacity_in_elements) noexcept = 0;

  virtual size_t get_blocks(const AbstractBlock **, uint8_t *,
                            size_t capacity_in_elements) const noexcept = 0;

  virtual bool contains(const AbstractBlock *) const noexcept = 0;
};

class Kernel {
 public:
  Kernel();
  // virtual ~Kernel() {};

 public:
  /// function ptr to window object
  virtual void setWindPtr(void *) = 0;
  /// a function ptr to show progress of converting and exporting
  virtual void setProgressRangeSet(void (*)(void *, int, int, int)) = 0;
  /// a function ptr to add progress value
  virtual void setProgressAdd(void (*)(void *, int)) = 0;
  /// a function ptr to prevent window from being syncoped
  virtual void setKeepAwake(void (*)(void *)) = 0;

  /// a function ptr to show progress of compressing and bridge-building
  virtual void setAlgoProgressRangeSet(void (*)(void *, int, int, int)) = 0;
  /// a function ptr to add progress value of compressing and bridge-building
  virtual void setAlgoProgressAdd(void (*)(void *, int)) = 0;

  /// a function ptr to report error when something wrong happens
  virtual void setReportError(void (*)(void *, ::SCL_errorFlag,
                                       const char *)) = 0;
  /// a function ptr to report working statue especially when busy
  virtual void setReportWorkingStatue(void (*)(void *, ::SCL_workStatues)) = 0;

  virtual void setAiCvterOpt(const AiCvterOpt *) = 0;

  virtual const AiCvterOpt *aiCvterOpt() const = 0;

  // can do in nothing:
  /// real size of kernel
  virtual unsigned long long size() = 0;
  /// revert to a previous step
  virtual void decreaseStep(::SCL_step) = 0;

  // can do in colorSetReady:
  /// get current step
  virtual ::SCL_step queryStep() const = 0;
  /// set map type and blocklist
  virtual bool setType(::SCL_mapTypes, ::SCL_gameVersion, const bool[64],
                       const AbstractBlock *const *const) = 0;
  /// get palette (base colors only) in ARGB32
  virtual void getBaseColorInARGB32(uint32_t *const) const = 0;

  // can do in wait4Image:
  /// set original image from ARGB32 matrix (col-major)
  [[deprecated]] virtual void setRawImage(const uint32_t *src, int rows,
                                          int cols) = 0;
  /// get accessible color count
  virtual unsigned short getColorCount() const = 0;
  /// get usable colors in ARGB32
  virtual void getAvailableColors(uint32_t *const ARGB32_dest = nullptr,
                                  uint8_t *const map_color_dest = nullptr,
                                  int *const num = nullptr) const = 0;
  /// make a structure that includes all accessible blocks
  [[deprecated]] virtual bool makeTests(const AbstractBlock **, const uint8_t *,
                                        const char *, char *) = 0;

  // can do in convertionReady:
  /// convert original image to map
  virtual bool convert(::SCL_convertAlgo = ::SCL_convertAlgo::RGB_Better,
                       bool dither = false) = 0;
  /// get image rows
  virtual int getImageRows() const = 0;
  /// get image cols
  virtual int getImageCols() const = 0;
  virtual const uint32_t *getRawImage() const = 0;
  /// query if map is buildable in vanilla survival
  virtual bool isVanilla() const = 0;
  /// query if map is a flat one
  virtual bool isFlat() const = 0;

  // can do in converted:
  /// construct 3D structure
  [[deprecated]] virtual bool build(
      ::SCL_compressSettings = ::SCL_compressSettings::noCompress,
      uint16_t maxAllowedHeight = 256,
      ::SCL_glassBridgeSettings = ::SCL_glassBridgeSettings::noBridge,
      uint16_t glassInterval = 3, bool fireProof = false,
      bool endermanProof = false) = 0;

  /// get converted image
  [[deprecated]] virtual void getConvertedImage(int *rows, int *cols,
                                                uint32_t *dest) const = 0;
  /// export as map data files, returns failed files.
  virtual void exportAsData(const char *FolderPath, const int indexStart,
                            int *fileCount, char **dest) const = 0;
  /// get converted map(in mapColor array)
  virtual void getConvertedMap(int *rows, int *cols, uint8_t *) const = 0;

  // can do in builded:
  /// export map into litematica files (*.litematic)
  [[deprecated]] virtual void exportAsLitematic(
      const char *localEncoding_TargetName, const char *utf8_LiteName,
      const char *utf8_RegionName, char *localEncoding_returnVal) const = 0;

  /// export map into Structure files (*.NBT)
  [[deprecated]] virtual void exportAsStructure(
      const char *localEncoding_TargetName,
      char *localEncoding_FileName) const = 0;
  [[deprecated]] virtual void exportAsWESchem(
      const char *localEncoding_fileName, const int (&offset)[3] = {0, 0, 0},
      const int (&weOffset)[3] = {0, 0, 0}, const char *utf8_Name = "",
      const char *const *const utf8_requiredMods = nullptr,
      const int requiredModsCount = 0,
      char *localEncoding_returnVal = nullptr) const = 0;

  /// get x,y,z size
  virtual void get3DSize(int *x, int *y, int *z) const = 0;

  /// get 3d structure's size
  virtual int getHeight() const = 0;
  /// get 3d structure's size
  virtual int getXRange() const = 0;
  /// get 3d structure's size
  virtual int getZRange() const = 0;
  /// get block count in total and in detail
  virtual void getBlockCounts(int *total, int detail[64]) const = 0;
  /// get sum block count
  virtual int64_t getBlockCounts() const = 0;
  /// get 3d structure in 3d-matrix (col major)
  virtual const uint16_t *getBuild(int *xSize = nullptr, int *ySize = nullptr,
                                   int *zSize = nullptr) const = 0;

  // Virtual functions added after v5.0.0. Define them in the end of vtable to
  // matain binary compability

  // introduced in v5.1.0  -----------------------------------------------------
  virtual void setCacheDir(const char *) noexcept = 0;
  virtual const char *cacheDir() const noexcept = 0;
  // replacement for setRawImage(const uint32_t*,int,int)
  virtual void setRawImage(const uint32_t *src, int rows, int cols,
                           bool is_col_major) = 0;
  // replacement for getConvertedImage(int *rows, int *cols, uint32_t
  // *dest)const
  virtual void getConvertedImage(int *rows, int *cols, uint32_t *dest,
                                 bool expected_col_major) const = 0;
  struct build_options {
    uint64_t version{SC_VERSION_U64};
    uint16_t maxAllowedHeight{256};
    uint16_t bridgeInterval{3};
    compressSettings compressMethod{::SCL_compressSettings::noCompress};
    glassBridgeSettings glassMethod{::SCL_glassBridgeSettings::noBridge};
    bool fire_proof{false};
    bool enderman_proof{false};

    // added in v5.1.0
    bool connect_mushrooms{false};
  };
  virtual bool build(const build_options &option) noexcept = 0;

  virtual void getCompressedImage(int *row, int *cols, uint32_t *dest,
                                  bool expected_col_major) const noexcept = 0;
  // requires step >= wait4image
  // virtual bool checkColorsetHash() const noexcept = 0;
  // requires step >= converted
  virtual bool saveConvertCache(StringDeliver &err) const noexcept = 0;

  // requires step >= convertion ready
  virtual bool loadConvertCache(SCL_convertAlgo algo, bool dither) noexcept = 0;

  // requires step >= built
  virtual bool saveBuildCache(StringDeliver &err) const noexcept = 0;

  // requires step >= converted
  virtual bool loadBuildCache(const build_options &option) noexcept = 0;

  // requires step >= built
  virtual int getSchemPalette(const char **dest_id,
                              size_t dest_capacity) const noexcept = 0;

  struct litematic_options {
    uint64_t version{SC_VERSION_U64};
    const char *litename_utf8 = "by SlopeCraft";
    const char *region_name_utf8 = "by SlopeCraft";
  };
  virtual bool exportAsLitematic(
      const char *filename_local,
      const litematic_options &option) const noexcept = 0;

  struct vanilla_structure_options {
    uint64_t version{SC_VERSION_U64};
    bool is_air_structure_void{true};
  };
  virtual bool exportAsStructure(
      const char *filename_local,
      const vanilla_structure_options &option) const noexcept = 0;

  struct WE_schem_options {
    uint64_t version{SC_VERSION_U64};
    int offset[3] = {0, 0, 0};
    int we_offset[3] = {0, 0, 0};
    const char *const *required_mods_name_utf8{nullptr};
    int num_required_mods{0};
  };
  virtual bool exportAsWESchem(
      const char *filename_local,
      const WE_schem_options &option) const noexcept = 0;

  struct flag_diagram_options {
    uint64_t version{SC_VERSION_U64};

    // 0 or negative number means no split lines
    int32_t split_line_row_margin{0};
    // 0 or negative number means no split lines
    int32_t split_line_col_margin{0};
    int png_compress_level{9};
    int png_compress_memory_level{8};
    StringDeliver *err{nullptr};
  };
  virtual bool exportAsFlatDiagram(
      const char *filename_local,
      const flag_diagram_options &option) const noexcept = 0;

  struct test_blocklist_options {
    uint64_t version{SC_VERSION_U64};
    const AbstractBlock *const *block_ptrs{nullptr};
    const uint8_t *basecolors{nullptr};
    size_t block_count{0};
    StringDeliver *err{nullptr};
  };
  virtual bool makeTests(
      const char *filename,
      const test_blocklist_options &option) const noexcept = 0;
};

}  // namespace SlopeCraft

// these functions are
extern "C" {
namespace SlopeCraft {

[[nodiscard]] SCL_EXPORT Kernel *SCL_createKernel();
SCL_EXPORT void SCL_destroyKernel(Kernel *);

[[nodiscard]] SCL_EXPORT AbstractBlock *SCL_createBlock();
SCL_EXPORT void SCL_destroyBlock(AbstractBlock *);

struct BlockListCreateOption {
  uint64_t version{SC_VERSION_U64};
  StringDeliver *warnings{nullptr};
  StringDeliver *error{nullptr};
};

[[nodiscard]] SCL_EXPORT BlockListInterface *SCL_createBlockList(
    const char *zip_filename, const BlockListCreateOption &option);

SCL_EXPORT void SCL_destroyBlockList(BlockListInterface *);

[[deprecated]] [[nodiscard]] SCL_EXPORT AiCvterOpt *SCL_createAiCvterOpt();
[[deprecated]] SCL_EXPORT void SCL_destroyAiCvterOpt(AiCvterOpt *);

[[deprecated]] SCL_EXPORT void SCL_setPopSize(AiCvterOpt *, uint32_t p);
[[deprecated]] SCL_EXPORT void SCL_setMaxGeneration(AiCvterOpt *, uint32_t p);
[[deprecated]] SCL_EXPORT void SCL_setMaxFailTimes(AiCvterOpt *, uint32_t p);
[[deprecated]] SCL_EXPORT void SCL_setCrossoverProb(AiCvterOpt *, double p);
[[deprecated]] SCL_EXPORT void SCL_setMutationProb(AiCvterOpt *, double p);

[[deprecated]] SCL_EXPORT uint32_t SCL_getPopSize(const AiCvterOpt *);
[[deprecated]] SCL_EXPORT uint32_t SCL_getMaxGeneration(const AiCvterOpt *);
[[deprecated]] SCL_EXPORT uint32_t SCL_getMaxFailTimes(const AiCvterOpt *);
[[deprecated]] SCL_EXPORT double SCL_getCrossoverProb(const AiCvterOpt *);
[[deprecated]] SCL_EXPORT double SCL_getMutationProb(const AiCvterOpt *);

SCL_EXPORT void SCL_preprocessImage(
    uint32_t *ARGB32ptr, const uint64_t imageSize,
    const SCL_PureTpPixelSt = SCL_PureTpPixelSt::ReplaceWithBackGround,
    const SCL_HalfTpPixelSt = SCL_HalfTpPixelSt::ComposeWithBackGround,
    uint32_t backGround = 0xFFFFFFFF);

SCL_EXPORT bool SCL_haveTransparentPixel(const uint32_t *ARGB32,
                                         const uint64_t imageSize);

SCL_EXPORT uint8_t SCL_maxAvailableVersion();

SCL_EXPORT const char *SCL_getSCLVersion();

SCL_EXPORT void SCL_getColorMapPtrs(const float **const rdata,
                                    const float **const gdata,
                                    const float **const bdata, const uint8_t **,
                                    int *);
// full palette
SCL_EXPORT const float *SCL_getBasicColorMapPtrs();

SCL_EXPORT SCL_gameVersion SCL_basecolor_version(uint8_t basecolor);
SCL_EXPORT uint8_t SCL_maxBaseColor();
SCL_EXPORT int SCL_getBlockPalette(const AbstractBlock **blkpp,
                                   size_t capacity_in_elements);

// SCL_EXPORT uint64_t SCL_mcVersion2VersionNumber(::SCL_gameVersion);

}  //  namespace SlopeCraft

}  //  extern "C"

// SCL_EXPORT void SCL_test();

#endif  // KERNEL_H
