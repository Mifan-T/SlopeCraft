#include "BlockListManager.h"
#include <string_view>
#include <QMessageBox>
#include <QDir>

extern std::string_view basecolor_names[64];

BlockListManager::BlockListManager(QWidget *parent) : QWidget(parent) {}

BlockListManager::~BlockListManager() {}

void BlockListManager::setup_basecolors(
    const SlopeCraft::Kernel *kernel) noexcept {
  this->basecolor_widgets.clear();
  this->basecolor_widgets.reserve(64);
  const int max_basecolor = SlopeCraft::SCL_maxBaseColor();

  uint32_t bc_arr[64];

  kernel->getBaseColorInARGB32(bc_arr);

  for (int bc = 0; bc <= max_basecolor; bc++) {
    std::unique_ptr<BaseColorWidget> bcw{new BaseColorWidget(this, bc)};
    this->layout()->addWidget(bcw.get());
    bcw->setTitle(QString::fromUtf8(basecolor_names[bc].data()));
    bcw->set_color(bc_arr[bc]);

    connect(bcw.get(), &BaseColorWidget::changed,
            [this]() { emit this->changed(); });
    this->basecolor_widgets.push_back(std::move(bcw));
  }
}

// bool callback_load_image(const char *filename, uint32_t *dst_row_major) {
//   QImage img(QString::fromLocal8Bit(filename));
//
//   if (img.isNull()) {
//     return false;
//   }
//
//   QImage another = img.convertedTo(QImage::Format_ARGB32).scaled(16, 16);
//
//   memcpy(dst_row_major, another.scanLine(0), 16 * 16 * sizeof(uint32_t));
//   return true;
// }

std::unique_ptr<SlopeCraft::BlockListInterface, BlockListDeleter>
BlockListManager::impl_addblocklist(const QString &filename) noexcept {
  std::string errmsg;
  errmsg.resize(8192);
  auto sd_err = SlopeCraft::StringDeliver::from_string(errmsg);
  std::string warning;
  warning.resize(8192);
  auto sd_warn = SlopeCraft::StringDeliver::from_string(warning);
  SlopeCraft::BlockListCreateOption option{SC_VERSION_U64, &sd_warn, &sd_err};

  SlopeCraft::BlockListInterface *bli =
      SlopeCraft::SCL_createBlockList(filename.toLocal8Bit().data(), option);

  errmsg.resize(sd_err.size);
  warning.resize(sd_warn.size);

  if (!errmsg.empty()) {
    errmsg.append(QStringLiteral("\npwd: %1")
                      .arg(QFileInfo{"."}.absolutePath())
                      .toLocal8Bit());
    if (bli == nullptr) {
      QMessageBox::critical(dynamic_cast<QWidget *>(this->parent()),
                            tr("解析方块列表失败"),
                            QString::fromUtf8(errmsg.data()));
      return {nullptr};
    } else {
      QMessageBox::warning(dynamic_cast<QWidget *>(this->parent()),
                           tr("解析方块列表失败"),
                           QString::fromUtf8(warning.data()));
    }
  }

  std::vector<SlopeCraft::AbstractBlock *> blockps;
  std::vector<uint8_t> base_colors;
  base_colors.resize(bli->size());
  blockps.resize(bli->size());

  bli->get_blocks(blockps.data(), base_colors.data(), blockps.size());

  for (size_t idx = 0; idx < bli->size(); idx++) {
    this->basecolor_widgets[base_colors[idx]]->add_block(blockps[idx]);
  }

  return std::unique_ptr<SlopeCraft::BlockListInterface, BlockListDeleter>{bli};
}

bool BlockListManager::add_blocklist(QString filename) noexcept {
  std::unique_ptr<SlopeCraft::BlockListInterface, BlockListDeleter> tmp =
      this->impl_addblocklist(filename);

  if (!tmp) {
    return false;
  }

  this->blockslists.emplace_back(std::move(tmp));

  return true;
}

void BlockListManager::finish_blocklist() noexcept {
  for (auto &bcw : this->basecolor_widgets) {
    bcw->finish_blocks();
  }
}

void BlockListManager::when_version_updated() noexcept {
  for (auto &bcw : this->basecolor_widgets) {
    bcw->when_version_updated(this->callback_get_version());
  }
}

void BlockListManager::get_blocklist(
    std::vector<uint8_t> &enable_list,
    std::vector<const SlopeCraft::AbstractBlock *> &block_list) const noexcept {
  enable_list.resize(64);
  block_list.resize(64);

  for (int bc = 0; bc < (int)this->basecolor_widgets.size(); bc++) {
    enable_list[bc] = this->basecolor_widgets[bc]->is_enabled();
    block_list[bc] = this->basecolor_widgets[bc]->selected_block();
  }

  for (int bc = (int)this->basecolor_widgets.size(); bc < 64; bc++) {
    enable_list[bc] = 0;
    block_list[bc] = nullptr;
  }
}

bool BlockListManager::loadPreset(const blockListPreset &preset) noexcept {
  if (preset.values.size() != this->basecolor_widgets.size()) {
    QMessageBox::warning(dynamic_cast<QWidget *>(this->parent()),
                         tr("加载预设错误"),
                         tr("预设文件包含的基色数量 (%1) 与实际情况 (%2) 不符")
                             .arg(preset.values.size())
                             .arg(this->basecolor_widgets.size()));
    return false;
  }

  for (int bc = 0; bc < (int)preset.values.size(); bc++) {
    auto &bcw = this->basecolor_widgets[bc];

    bcw->set_enabled(preset.values[bc].first);

    auto &bws = bcw->block_widgets();
    int matched_idx = -1;
    for (int idx = 0; idx < (int)bws.size(); idx++) {
      if (QString::fromLatin1(bws[idx]->attachted_block()->getId()) ==
          preset.values[bc].second) {
        matched_idx = idx;
        break;
      }
    }

    if (matched_idx < 0) {
      auto ret = QMessageBox::warning(
          dynamic_cast<QWidget *>(this->parent()), tr("加载预设错误"),
          tr("预设中为基色%1指定的方块 id 是\"%2\"，没有找到这个方块 id")
              .arg(bc)
              .arg(preset.values[bc].second),
          QMessageBox::StandardButtons{QMessageBox::StandardButton::Ignore,
                                       QMessageBox::StandardButton::Close});
      if (ret == QMessageBox::StandardButton::Close) {
        exit(__LINE__);
        // return false;
      }
      continue;
    }

    bcw->select_block_soft(matched_idx);
  }

  // emit this->changed();

  return true;
}

blockListPreset BlockListManager::to_preset() const noexcept {
  blockListPreset ret;
  ret.values.resize(this->basecolor_widgets.size());
  for (size_t basecolor = 0; basecolor < this->basecolor_widgets.size();
       basecolor++) {
    ret.values[basecolor].first =
        this->basecolor_widgets[basecolor]->is_enabled();
    ret.values[basecolor].second = QString::fromUtf8(
        this->basecolor_widgets[basecolor]->selected_block()->getId());
  }
  return ret;
}

std::string_view basecolor_names[64] = {"00 None",
                                        "01 Grass",
                                        "02 Sand",
                                        "03 Wool",
                                        "04 Fire",
                                        "05 Ice",
                                        "06 Metal",
                                        "07 Plant",
                                        "08 Snow",
                                        "09 Clay",
                                        "10 Dirt",
                                        "11 Stone",
                                        "12 Water",
                                        "13 Wood",
                                        "14 Quartz",
                                        "15 ColorOrange",
                                        "16 ColorMagenta",
                                        "17 ColorLightBlue",
                                        "18 ColorYellow",
                                        "19 ColorLime",
                                        "20 ColorPink",
                                        "21 ColorGray",
                                        "22 ColorLightGray",
                                        "23 ColorCyan",
                                        "24 ColorPurple",
                                        "25 ColorBlue",
                                        "26 ColorBrown",
                                        "27 ColorGreen",
                                        "28 ColorRed",
                                        "29 ColorBlack",
                                        "30 Gold",
                                        "31 Diamond",
                                        "32 Lapis",
                                        "33 Emerald",
                                        "34 Podzol",
                                        "35 Nether",
                                        "36 TerracottaWhite",
                                        "37 TerracottaOrange",
                                        "38 TerracottaMagenta",
                                        "39 TerracottaLightBlue",
                                        "40 TerracottaYellow",
                                        "41 TerracottaLime",
                                        "42 TerracottaPink",
                                        "43 TerracottaGray",
                                        "44 TerracottaLightGray",
                                        "45 TerracottaCyan",
                                        "46 TerracottaPurple",
                                        "47 TerracottaBlue",
                                        "48 TerracottaBrown",
                                        "49 TerracottaGreen",
                                        "50 TerracottaRed",
                                        "51 TerracottaBlack",
                                        "52 CrimsonNylium",
                                        "53 CrimsonStem",
                                        "54 CrimsonHyphae",
                                        "55 WarpedNylium",
                                        "56 WarpedStem",
                                        "57 WarpedHyphae",
                                        "58 WarpedWartBlock",
                                        "59 Deepslate",
                                        "60 RawIron",
                                        "61 GlowLichen",
                                        "Unknown",
                                        "Unknown"};