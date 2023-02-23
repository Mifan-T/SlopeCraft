#include "DirectionHandler.hpp"
#include "TokiVC.h"
#include "VCL_internal.h"
#include "VisualCraftL.h"

void TokiVC::fill_schem_blocklist_no_lock() noexcept {
  std::vector<const char *> blk_ids;
  // fill with nullptr
  blk_ids.resize(TokiVC::blocks_allowed.size());
  for (auto &p : blk_ids) {
    p = nullptr;
  }
  for (const auto &pair : TokiVC::blocks_allowed) {
    blk_ids[pair.second] = pair.first->full_id_ptr()->c_str();
  }
  this->schem.set_block_id(blk_ids.data(), blk_ids.size());
}

bool TokiVC::build() noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (this->_step < VCL_Kernel_step::VCL_wait_for_build) {
    VCL_report(VCL_report_type_t::error,
               "Trying to build before without image converted.");
    return false;
  }

  dir_handler<int64_t> dirh(TokiVC::exposed_face, this->img_cvter.rows(),
                            this->img_cvter.cols(), TokiVC::max_block_layers);

  this->schem.resize(dirh.range_xyz()[0], dirh.range_xyz()[1],
                     dirh.range_xyz()[2]);

  this->fill_schem_blocklist_no_lock();

  this->schem.fill(0);

  Eigen::ArrayXX<uint16_t> color_id_mat = this->img_cvter.color_id();

  for (int64_t r = 0; r < this->img_cvter.rows(); r++) {
    for (int64_t c = 0; c < this->img_cvter.cols(); c++) {
      const uint16_t color_id = color_id_mat(r, c);
      const auto variant = TokiVC::LUT_basic_color_idx_to_blocks[color_id];

      const VCL_block *const *blockpp = nullptr;
      size_t depth_current = 0;

      if (variant.index() == 0) {
        blockpp = &std::get<const VCL_block *>(variant);
        depth_current = 1;
      } else {
        const auto &vec = std::get<std::vector<const VCL_block *>>(variant);
        blockpp = vec.data();
        depth_current = vec.size();
      }

      for (size_t depth = 0; depth < depth_current; depth++) {
        const auto coord = dirh.coordinate_of(r, c, depth);
        const VCL_block *const blkp = blockpp[depth];

        auto it = TokiVC::blocks_allowed.find(blkp);
        if (it == TokiVC::blocks_allowed.end()) {
          std::string msg =
              fmt::format("Failed to find VCL_block at address {} named {} in "
                          "allowed blocks. This is an internal error.",
                          (const void *)blkp, blkp->full_id_ptr()->c_str());
          VCL_report(VCL_report_type_t::error, msg.c_str());
          return false;
        }

        const uint16_t blk_id = it->second;

        this->schem(coord[0], coord[1], coord[2]) = blk_id;
      }
    }
    // finish a coloum
  }

  this->_step = VCL_Kernel_step::VCL_built;

  return true;
}

int64_t TokiVC::xyz_size(int64_t *x, int64_t *y, int64_t *z) const noexcept {
  std::shared_lock<std::shared_mutex> lkgd(TokiVC_internal::global_lock);

  if (x != nullptr)
    *x = this->schem.x_range();
  if (y != nullptr)
    *y = this->schem.y_range();
  if (z != nullptr)
    *z = this->schem.z_range();

  return this->schem.size();
}