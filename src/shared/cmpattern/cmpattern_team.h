//========================================================================
//  This software is free: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License Version 3,
//  as published by the Free Software Foundation.
//
//  This software is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  Version 3 in the file COPYING that came with this distribution.
//  If not, see <http://www.gnu.org/licenses/>.
//========================================================================
/*!
  \file    cmpattern_team.h
  \brief   C++ Interface: team
  \author  Stefan Zickler, 2009
*/
//========================================================================
#ifndef CM_PATTERN_TEAM_H
#define CM_PATTERN_TEAM_H
#include <QObject>
#include "VarTypes.h"
#include "geometry.h"
#include "VarNotifier.h"
using namespace VarTypes;
namespace CMPattern {

/**
	@author Author Name
*/
class TeamSelector;
class TeamDetector;

class Team : public QObject {
Q_OBJECT

friend class TeamSelector;
friend class TeamDetector;
signals:
   void signalTeamNameChanged();
signals:
   void signalChangeOccured(VarType * item);
protected slots:
   void slotTeamNameChanged();
   void slotChangeOccured(VarType * item);
protected:
  VarTypes::VarNotifier _notifier;
  VarTypes::VarList * _settings;
  VarTypes::VarString * _team_name;
  VarTypes::VarBool * _unique_patterns;
  VarTypes::VarBool * _have_angle;
  VarTypes::VarBool * _load_markers_from_image_file;
  VarTypes::VarString * _marker_image_file;
  VarTypes::VarInt *    _marker_image_rows;
  VarTypes::VarInt *    _marker_image_cols;
  VarTypes::VarSelection * _valid_patterns;
  VarTypes::VarDouble * _robot_height;
  VarTypes::VarBool   * _use_marker_image_heights;
  VarTypes::VarList * _marker_image;

  VarTypes::VarList * _center_marker_filter;
  VarTypes::VarDouble * _center_marker_area_mean;
  VarTypes::VarDouble * _center_marker_area_stddev;
  VarTypes::VarDouble * _center_marker_uniform;
  VarTypes::VarInt * _center_marker_min_width;
  VarTypes::VarInt * _center_marker_max_width;
  VarTypes::VarInt * _center_marker_min_height;
  VarTypes::VarInt * _center_marker_max_height;
  VarTypes::VarInt * _center_marker_min_area;
  VarTypes::VarInt * _center_marker_max_area;
  VarTypes::VarInt * _center_marker_duplicate_distance;

  VarTypes::VarList * _other_markers_filter;
  VarTypes::VarInt * _other_markers_min_width;
  VarTypes::VarInt * _other_markers_max_width;
  VarTypes::VarInt * _other_markers_min_height;
  VarTypes::VarInt * _other_markers_max_height;
  VarTypes::VarInt * _other_markers_min_area;
  VarTypes::VarInt * _other_markers_max_area;

  VarTypes::VarList * _histogram_settings;
  VarTypes::VarBool * _histogram_enable;
  VarTypes::VarInt * _histogram_pixel_scan_radius;
  VarTypes::VarDouble * _histogram_min_markeryness;
  VarTypes::VarDouble * _histogram_max_markeryness;
  VarTypes::VarDouble * _histogram_min_field_greenness;
  VarTypes::VarDouble * _histogram_max_field_greenness;
  VarTypes::VarDouble * _histogram_min_black_whiteness;
  VarTypes::VarDouble * _histogram_max_black_whiteness;

  VarTypes::VarList * _pattern_fitness;
  VarTypes::VarDouble * _pattern_max_dist;
  VarTypes::VarDouble * _pattern_fitness_weight_area;
  VarTypes::VarDouble * _pattern_fitness_weight_center_distance;
  VarTypes::VarDouble * _pattern_fitness_weight_next_distance;
  VarTypes::VarDouble * _pattern_fitness_weight_next_angle_distance;
  VarTypes::VarDouble * _pattern_fitness_max_error;
  VarTypes::VarDouble * _pattern_fitness_stddev;
  VarTypes::VarDouble * _pattern_fitness_uniform;

public:
    Team(VarList * team_root);

    ~Team();

};

}
#endif
