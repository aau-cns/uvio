#ifndef UVIOMANAGEROPTIONS_H
#define UVIOMANAGEROPTIONS_H

#include "core/VioManagerOptions.h"
#include "state/UVioStateOptions.h"
#include "update/UVioUpdaterOptions.h"
#include "utils/uvio_sensor_data.h"

namespace uvio {

/**
 * @brief Struct which stores uvio specific options
 */
struct UVioManagerOptions : ov_msckf::VioManagerOptions {

  /// Number of anchors
  int n_anchors = 0;

  /// Number of fixed anchors
  int n_anchors_to_fix = 0;

  /// Minimum traveled distance to initialize anchors
  double min_dist_to_use_uwb = 0.0;

  /// uwb extrinsics (p_IinU).
  Eigen::Vector3d uwb_extrinsics = Eigen::Vector3d::Zero();

  /// offset between initial position of UAV and global frame (p_GinI0).
  Eigen::Vector3d offset_p0 = Eigen::Vector3d::Zero();

  /// uwb anchors references (id, p_AinG, const_bias, dist_bias, Cov).
  std::vector<AnchorData> uwb_anchors;

  /// UWB options
  UVioUpdaterOptions uwb_options;

  /// UVIO specific options
  UVioStateOptions uvio_state_options;

  /**
   * @brief This function will load the non-simulation parameters of the system and print.
   * @param parser If not null, this parser will be used to load our parameters
   */
  void print_and_load(const std::shared_ptr<ov_core::YamlParser> &parser = nullptr) {

    ov_msckf::VioManagerOptions::print_and_load(parser);

    PRINT_DEBUG("\n\nUVIO PARAMETERS:\n");

    if (parser != nullptr) {
      /// Parse number of anchors
      parser->parse_external("config_uwb", "init", "n_fixed_anchors", n_anchors_to_fix);
      parser->parse_external("config_uwb", "init", "n_known_anchors", n_anchors);
      parser->parse_external("config_uwb", "init", "min_dist_to_use_uwb", min_dist_to_use_uwb);

      /// Calibration parameters
      std::vector<double> p_UinI = {0, 0, 0};
      parser->parse_external("config_uwb", "tag0", "p_UinI", p_UinI);
      uwb_extrinsics << -p_UinI.at(0), -p_UinI.at(1), -p_UinI.at(2);

      /// Initial offset parameter
      std::vector<double> p_IinG0 = {0, 0, 0};
      parser->parse_external("config_uwb", "tag0", "p_IinG0", p_IinG0);
      offset_p0 << -p_IinG0.at(0), -p_IinG0.at(1), -p_IinG0.at(2);

      /// Parse anchors
      for (int i = 0; i < n_anchors; i++) {
        int anch_id;
        bool anch_fix;
        double const_b, dist_b, p, c, d;
        std::vector<double> pos = {0, 0, 0};
        parser->parse_external("uwb_anchors", "anchor" + std::to_string(i), "id", anch_id);
        parser->parse_external("uwb_anchors", "anchor" + std::to_string(i), "fix", anch_fix);
        parser->parse_external("uwb_anchors", "anchor" + std::to_string(i), "p_AinG", pos);
        parser->parse_external("uwb_anchors", "anchor" + std::to_string(i), "const_bias", const_b);
        parser->parse_external("uwb_anchors", "anchor" + std::to_string(i), "dist_bias", dist_b);
        parser->parse_external("uwb_anchors", "anchor" + std::to_string(i), "prior_p_AinG_cov", p);
        parser->parse_external("uwb_anchors", "anchor" + std::to_string(i), "prior_const_bias_cov", c);
        parser->parse_external("uwb_anchors", "anchor" + std::to_string(i), "prior_dist_bias_cov", d);

        AnchorData anchor;
        anchor.id = anch_id;
        anchor.fix = anch_fix;
        anchor.p_AinG << pos.at(0) - p_IinG0.at(0), pos.at(1) - p_IinG0.at(1), pos.at(2) - p_IinG0.at(2);
        anchor.const_bias = const_b;
        anchor.dist_bias = dist_b;
        anchor.cov.diagonal() << p, p, p, c, d;

        uwb_anchors.push_back(anchor);
      }
    }

    PRINT_DEBUG("  - n_anchors: %d\n", n_anchors);
    PRINT_DEBUG("  - n_anchors_to_fix: %d\n", n_anchors_to_fix);
    PRINT_DEBUG("  - min_dist_to_init_anchors: %.3f\n", min_dist_to_use_uwb);
    PRINT_DEBUG("  - calib_uwb_imu: [%.3f, %.3f, %.3f]\n", uwb_extrinsics(0), uwb_extrinsics(1), uwb_extrinsics(2));
    PRINT_DEBUG("  - p_GinI0: [%.3f, %.3f, %.3f]\n", offset_p0(0), offset_p0(1), offset_p0(2));

    uvio_state_options.print_and_load(parser);
    uwb_options.print_and_load(parser);

    for (size_t i = 0; i < uwb_anchors.size(); i++) {
      PRINT_DEBUG("  - anchor[%d]: fixed = %s\n", uwb_anchors.at(i).id, uwb_anchors.at(i).fix ? "true" : "false");
      PRINT_DEBUG("  - anchor[%d]: p_AinG = [%.3f, %.3f, %.3f]\n", uwb_anchors.at(i).id, uwb_anchors.at(i).p_AinG.x(),
                  uwb_anchors.at(i).p_AinG.y(), uwb_anchors.at(i).p_AinG.z());
      PRINT_DEBUG("  - anchor[%d]: const_bias = %.3f\n", uwb_anchors.at(i).id, uwb_anchors.at(i).const_bias);
      PRINT_DEBUG("  - anchor[%d]: dist_bias = %.3f\n", uwb_anchors.at(i).id, uwb_anchors.at(i).dist_bias);
      PRINT_DEBUG("  - anchor[%d]: cov.diagonal() = [%.3f, %.3f, %.3f, %.4f, %.4f]\n\n", uwb_anchors.at(i).id,
                  uwb_anchors.at(i).cov.diagonal()(0), uwb_anchors.at(i).cov.diagonal()(1), uwb_anchors.at(i).cov.diagonal()(2),
                  uwb_anchors.at(i).cov.diagonal()(3), uwb_anchors.at(i).cov.diagonal()(4));
    }
  }
};

} // namespace uvio

#endif // UVIOMANAGEROPTIONS_H
