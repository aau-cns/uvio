/*
 * Copyright (C) 2020 Alessandro Fornasier, Control of Networked Systems, University of Klagenfurt, Austria.
 *
 * All rights reserved.
 *
 * You can contact the author at <alessandro.fornasier@aau.at>
 */

#include "UpdaterUWB.h"

using namespace ov_core;
using namespace bw2_ms_msckf;

void UpdaterUWB::update(std::shared_ptr<State> state, const std::shared_ptr<ov_core::UwbData>& message) {

  // Convert uwb measurement into our current format keeping only valid measuremetns
  UpdaterHelper::UpdaterHelperUWB meas;
  meas.uwbs = *message;
  meas.const_bias = state->_uwb_const_bias->value()(0);
  meas.distance_bias = state->_uwb_distance_bias->value()(0);

  // Copy Reference on the meas structure dereferenciating the pointer
  for (const auto &it : state->_calib_GLOBALtoANCHORS) {
    meas.anchors.insert({it.first, it.second->value()});
  }

  // Measurement noise for the uwb update
  // Alessandro: fixed value
  Eigen::MatrixXd R = pow(_options_uwb.uwb_sigma_range,2)*Eigen::MatrixXd::Identity(message->uwb_ranges.size(),message->uwb_ranges.size());

  // Our return values (state jacobian, residual, and order of state jacobian)
  Eigen::MatrixXd H_x;
  Eigen::VectorXd res;
  std::vector<std::shared_ptr<Type>> Hx_order;

  // Get the Jacobian for the uwb update
  UpdaterHelper::get_uwb_jacobian_full(state, meas, H_x, res, Hx_order);

//  printf(YELLOW "\nUWB JACOBIAN:\n" RESET);
//  std::cout << H_x << std::endl;

  // Chi2 distance check
  Eigen::MatrixXd P_marg = StateHelper::get_marginal_covariance(state, Hx_order);
  Eigen::MatrixXd S = H_x*P_marg*H_x.transpose() + R; //Innovation matrix
  double chi2 = res.dot(S.llt().solve(res)); //r^T(S^-1)r

  // Get our threshold (we precompute up to 500 but handle the case that it is more)
  double chi2_check = chi_squared_table[res.rows()];

  // Check if we should update or not
  // Alessandro: [check] CHI2 test
  if(chi2 > _options_uwb.chi2_multipler*chi2_check) {
//      std::cout << "UWB update chi2 = " << chi2 << " > " << _options_uwb.chi2_multipler*chi2_check << std::endl;
      return;
  }

  // We are good! Perform measurement compression
  UpdaterHelper::measurement_compress_inplace(H_x, res);
  if(H_x.rows() < 1) {
    return;
  }

  // Update the state
  StateHelper::EKFUpdate(state, Hx_order, H_x, res, R);

}
