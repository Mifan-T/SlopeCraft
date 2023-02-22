#include <VisualCraftL.h>
#include <omp.h>

#include <CLI11.hpp>
#include <QImage>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

using std::cout, std::endl;

struct inputs {
  std::vector<std::string> zips;
  std::vector<std::string> jsons;
  std::vector<std::string> images;
  std::string prefix;
  int layers;
  int num_threads;
  SCL_gameVersion version;
  VCL_face_t face;
  bool make_converted_image{false};
  SCL_convertAlgo algo;
  bool dither{false};
  bool benchmark{false};
  bool prefer_gpu{false};
  bool show_gpu{false};
  uint8_t platform_idx{0};
  uint8_t device_idx{0};
  bool list_gpu{false};
};

int run(const inputs &input) noexcept;
int set_resource(VCL_Kernel *kernel, const inputs &input) noexcept;
int set_allowed(VCL_block_state_list *bsl, const inputs &input) noexcept;

SCL_convertAlgo str_to_algo(std::string_view str, bool &ok) noexcept;

void cb_progress_range_set(void *, int, int, int) {}
void cb_progress_add(void *, int) {}

int list_gpu();

int main(int argc, char **argv) {
  inputs input;
  CLI::App app;
  app.add_option("--rp", input.zips, "Resource packs")
      ->check(CLI::ExistingFile);
  app.add_option("--bsl", input.jsons, "Block state list json file.")
      ->check(CLI::ExistingFile);
  app.add_option("--img", input.images, "Images to convert")
      ->check(CLI::ExistingFile);

  app.add_option("--layers", input.layers, "Max layers")
      ->default_val(3)
      ->check(CLI::PositiveNumber);

  std::string __face;
  app.add_option("--face", __face, "Facing direction.")
      ->default_val("up")
      ->check(CLI::IsMember({"up", "down", "north", "south", "east", "west"}));

  int __version;
  app.add_option("--version", __version, "MC version.")
      ->default_val(19)
      ->check(CLI::Range(12, 19, "Avaliable versions."));

  app.add_option("--prefix", input.prefix, "Prefix to generate output")
      ->default_val("");

  app.add_flag("--out-image", input.make_converted_image,
               "Generate converted image")
      ->default_val(false);

  std::string algo;
  app.add_option("--algo", algo, "Algo for conversion")
      ->default_val("RGB_Better")
      ->check(CLI::IsMember(
          {"RGB", "RGB_Better", "HSV", "Lab94", "Lab00", "XYZ", "gaCvter"}))
      ->expected(1);
  app.add_flag("--dither", input.dither)->default_val(false);
  app.add_flag("--benchmark", input.benchmark, "Display the performance data.")
      ->default_val(false);

  app.add_option("-j", input.num_threads)
      ->check(CLI::PositiveNumber)
      ->default_val(std::thread::hardware_concurrency());

  app.add_flag("--gpu", input.prefer_gpu, "Use gpu firstly")
      ->default_val(false);

  app.add_option("--platform", input.platform_idx)
      ->default_val(0)
      ->check(CLI::NonNegativeNumber);
  app.add_option("--device", input.device_idx)
      ->default_val(0)
      ->check(CLI::NonNegativeNumber);
  app.add_flag("--show-gpu", input.show_gpu)->default_val(false);

  app.add_flag("--list-gpu", input.list_gpu,
               "List all GPU platforms and devices that OpenCL has access to.")
      ->default_val(false);

  CLI11_PARSE(app, argc, argv);

  input.version = SCL_gameVersion(__version);

  bool ok = true;

  input.face = VCL_str_to_face_t(__face.c_str(), &ok);
  if (!ok) {
    return __LINE__;
  }

  input.algo = str_to_algo(algo, ok);

  if (!ok) {
    return __LINE__;
  }

  if (input.list_gpu) {
    return list_gpu();
  }

  return run(input);
}

#define VCCL_PRIVATE_MACRO_MAKE_CASE(enum_val)                                 \
  if (str == #enum_val) {                                                      \
    ok = true;                                                                 \
    return SCL_convertAlgo::enum_val;                                          \
  }

SCL_convertAlgo str_to_algo(std::string_view str, bool &ok) noexcept {
  ok = false;

  VCCL_PRIVATE_MACRO_MAKE_CASE(RGB);
  VCCL_PRIVATE_MACRO_MAKE_CASE(RGB_Better);
  VCCL_PRIVATE_MACRO_MAKE_CASE(HSV);
  VCCL_PRIVATE_MACRO_MAKE_CASE(Lab94);
  VCCL_PRIVATE_MACRO_MAKE_CASE(Lab00);
  VCCL_PRIVATE_MACRO_MAKE_CASE(XYZ);
  VCCL_PRIVATE_MACRO_MAKE_CASE(gaCvter);

  return {};
}

int list_gpu() {
  const size_t plat_num = VCL_platform_num();
  cout << plat_num << " platforms found on this computer : \n";
  for (size_t pid = 0; pid < plat_num; pid++) {

    VCL_GPU_Platform *plat = VCL_get_platform(pid);
    if (plat == nullptr) {
      cout << "Failed to get platform " << pid << '\n';
      continue;
    }
    cout << "Platform " << pid << " : " << VCL_get_platform_name(plat) << '\n';

    const size_t dev_num = VCL_get_device_num(plat);
    for (size_t did = 0; did < dev_num; did++) {
      VCL_GPU_Device *dp = VCL_get_device(plat, did);
      if (dp == nullptr) {
        cout << "Failed to get device " << did << '\n';
        continue;
      }
      cout << "    Device " << did << " : " << VCL_get_device_name(dp) << '\n';
      VCL_release_device(dp);
    }
    VCL_release_platform(plat);
  }
  return 0;
}

int set_resource(VCL_Kernel *kernel, const inputs &input) noexcept {
  std::vector<const char *> zip_filenames, json_filenames;

  for (const auto &str : input.zips) {
    zip_filenames.emplace_back(str.c_str());
  }
  for (const auto &str : input.jsons) {
    json_filenames.emplace_back(str.c_str());
  }

  VCL_block_state_list *bsl =
      VCL_create_block_state_list(json_filenames.size(), json_filenames.data());

  if (bsl == nullptr) {
    cout << "Failed to parse block state list." << endl;
    VCL_destroy_kernel(kernel);
    return __LINE__;
  }

  VCL_resource_pack *rp =
      VCL_create_resource_pack(zip_filenames.size(), zip_filenames.data());

  if (rp == nullptr) {
    cout << "Failed to parse resource pack(s)." << endl;
    VCL_destroy_block_state_list(bsl);
    VCL_destroy_kernel(kernel);
    return __LINE__;
  }
  VCL_set_resource_option option;
  option.version = input.version;
  option.max_block_layers = input.layers;
  option.exposed_face = input.face;

  if (!VCL_set_resource_move(&rp, &bsl, option)) {
    cout << "Failed to set resource pack" << endl;
    VCL_destroy_block_state_list(bsl);
    VCL_destroy_resource_pack(rp);
    VCL_destroy_kernel(kernel);
    return __LINE__;
  }

  VCL_destroy_block_state_list(bsl);
  VCL_destroy_resource_pack(rp);
  return 0;
}

int set_allowed(VCL_block_state_list *bsl, const inputs &input) noexcept {
  std::vector<VCL_block *> blocks;

  blocks.resize(VCL_get_blocks_from_block_state_list_match(
      bsl, input.version, input.face, nullptr, 0));

  const int num = VCL_get_blocks_from_block_state_list_match(
      bsl, input.version, input.face, blocks.data(), blocks.size());

  if (int(blocks.size()) != num) {
    return __LINE__;
  }

  if (!VCL_set_allowed_blocks(blocks.data(), blocks.size())) {
    cout << "Failed to set allowed blocks." << endl;
    return __LINE__;
  }

  return 0;
}

int run(const inputs &input) noexcept {

  if (input.zips.size() <= 0 || input.jsons.size() <= 0) {
    cout << "No zips or jsons provided. exit." << endl;
    return __LINE__;
  }

  VCL_Kernel *kernel = VCL_create_kernel();
  if (kernel == nullptr) {
    cout << "Failed to create kernel." << endl;
    return __LINE__;
  }

  cout << "algo = " << (char)input.algo << endl;

  kernel->set_prefer_gpu(input.prefer_gpu);
  if (input.prefer_gpu) {
    const bool ok =
        kernel->set_gpu_resource(input.platform_idx, input.device_idx);
    if (!ok || !kernel->have_gpu_resource()) {
      cout << "Failed to set gpu resource for kernel. Platform and device may "
              "be invalid."
           << endl;
      return __LINE__;
    }

    if (input.show_gpu) {
      kernel->show_gpu_name();
    }
  }

  omp_set_num_teams(input.num_threads);

  kernel->set_ui(nullptr, cb_progress_range_set, cb_progress_add);

  {
    const int ret = set_resource(kernel, input);
    if (ret != 0) {
      return ret;
    }
  }

  {
    const int ret = set_allowed(VCL_get_block_state_list(), input);
    if (ret != 0) {
      return ret;
    }
  }

  for (const auto &img_filename : input.images) {
    QImage img(img_filename.c_str());

    if (img.isNull()) {
      cout << "Failed to open image " << img_filename << endl;
      VCL_destroy_kernel(kernel);
      return __LINE__;
    }

    img = img.convertToFormat(QImage::Format::Format_ARGB32);

    if (img.isNull()) {
      return __LINE__;
    }
    if (!kernel->set_image(img.height(), img.width(),
                           (const uint32_t *)img.scanLine(0), true)) {
      cout << "Failed to set raw image." << endl;
      return __LINE__;
    }

    double wt = omp_get_wtime();

    if (!kernel->convert(input.algo, input.dither)) {
      cout << "Failed to convert" << endl;
      return __LINE__;
    }
    wt = omp_get_wtime() - wt;

    if (input.benchmark) {
      cout << "Converting " << img.height() * img.width() << " pixels in " << wt
           << " seconds." << endl;
    }

    if (input.make_converted_image) {
      std::string dst_name_str(input.prefix);
      dst_name_str += std::filesystem::path(img_filename).filename().string();

      memset(img.scanLine(0), 0, img.height() * img.width() * sizeof(uint32_t));

      kernel->converted_image((uint32_t *)img.scanLine(0), nullptr, nullptr,
                              true);

      const bool ok = img.save(QString::fromLocal8Bit(dst_name_str.c_str()));

      if (!ok) {
        cout << "Failed to save image " << dst_name_str << endl;
        return __LINE__;
      }
      // cout << dst_path << endl;
    }
  }

  VCL_destroy_kernel(kernel);

  cout << "success." << endl;

  return 0;
}