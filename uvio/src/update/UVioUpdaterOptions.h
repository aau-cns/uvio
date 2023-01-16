/*
 * OpenVINS: An Open Platform for Visual-Inertial Research
 * Copyright (C) 2019 Patrick Geneva
 * Copyright (C) 2019 Kevin Eckenhoff
 * Copyright (C) 2019 Guoquan Huang
 * Copyright (C) 2020 Alessandro Fornasier, Control of Networked Systems, University of Klagenfurt, Austria (alessandro.fornasier@aau.at)
 * Copyright (C) 2020 OpenVINS Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UVIO_UPDATER_OPTIONS_H
#define UVIO_UPDATER_OPTIONS_H

#include "update/UpdaterOptions.h"

namespace uvio {

/**
     * @brief Struct which stores general updater options
     */
struct UVioUpdaterOptions : public ov_msckf::UpdaterOptions {

  /// Noise sigma for our uwb range measurement
  double uwb_sigma_range = 1;

  /// Nice print function of what parameters we have loaded
  void print() {
    ov_msckf::UpdaterOptions::print();
    PRINT_DEBUG("    - uwb_sigma_range: %.2f\n", uwb_sigma_range);
  }

};


}

#endif //UVIO_UPDATER_OPTIONS_H
