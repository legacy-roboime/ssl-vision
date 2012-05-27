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
  \file    field.h
  \brief   Definition of field dimensions
  \author  Stefan Zickler / Tim Laue, (C) 2009
*/
//========================================================================

#ifndef FIELD_H
#define FIELD_H
#include "field_default_constants.h"

#include "VarTypes.h"
#include <QObject>

#ifndef NO_PROTOBUFFERS
  #include "messages_robocup_ssl_geometry.pb.h"
#endif

using namespace VarTypes;

/*!
  \class RoboCupField

  \brief Definition of all variables for a symmetric, regulation-style RoboCup SSL field

  \author Stefan Zickler , (C) 2009
**/
class RoboCupField : public QObject
{
Q_OBJECT

protected:
  vector<VarType *> field_params;
  vector<VarType *> derived_params;
  VarList * settings;
  VarTrigger * restore;
public:
  VarList * getSettings() const {
    return settings;
  }
  VarTypes::VarInt * line_width;
  VarTypes::VarInt * field_length;
  VarTypes::VarInt * field_width;
  VarTypes::VarInt * boundary_width;
  VarTypes::VarInt * referee_width;
  VarTypes::VarInt * goal_width;
  VarTypes::VarInt * goal_depth;
  VarTypes::VarInt * goal_wall_width;
  VarTypes::VarInt * center_circle_radius;
  VarTypes::VarInt * defense_radius;
  VarTypes::VarInt * defense_stretch;
  VarTypes::VarInt * free_kick_from_defense_dist;
  VarTypes::VarInt * penalty_spot_from_field_line_dist;
  VarTypes::VarInt * penalty_line_from_spot_dist;

  //derived:
  VarTypes::VarInt * field_total_playable_length;
  VarTypes::VarInt * field_total_playable_width;
  VarTypes::VarInt * field_total_surface_length;
  VarTypes::VarInt * field_total_surface_width;
  VarTypes::VarInt * half_field_length;
  VarTypes::VarInt * half_field_width;
  VarTypes::VarInt * half_line_width;
  VarTypes::VarInt * half_goal_width;
  VarTypes::VarInt * half_defense_stretch;
  VarTypes::VarInt * half_field_total_playable_length;
  VarTypes::VarInt * half_field_total_playable_width;
  VarTypes::VarInt * half_field_total_surface_length;
  VarTypes::VarInt * half_field_total_surface_width;

  #ifndef NO_PROTOBUFFERS
  void toProtoBuffer(SSL_GeometryFieldSize & buffer) const {
    buffer.set_line_width(line_width->getInt());
    buffer.set_field_length(field_length->getInt());
    buffer.set_field_width(field_width->getInt());
    buffer.set_boundary_width(boundary_width->getInt());
    buffer.set_referee_width(referee_width->getInt());
    buffer.set_goal_width(goal_width->getInt());
    buffer.set_goal_depth(goal_depth->getInt());
    buffer.set_goal_wall_width(goal_wall_width->getInt());
    buffer.set_center_circle_radius(center_circle_radius->getInt());
    buffer.set_defense_radius(defense_radius->getInt());
    buffer.set_defense_stretch(defense_stretch->getInt());
    buffer.set_free_kick_from_defense_dist(free_kick_from_defense_dist->getInt());
    buffer.set_penalty_spot_from_field_line_dist(penalty_spot_from_field_line_dist->getInt());
    buffer.set_penalty_line_from_spot_dist(penalty_line_from_spot_dist->getInt());
  }

  void fromProtoBuffer(const SSL_GeometryFieldSize & buffer) {
    line_width->setInt(buffer.line_width());
    field_length->setInt(buffer.field_length());
    field_width->setInt(buffer.field_width());
    boundary_width->setInt(buffer.boundary_width());
    referee_width->setInt(buffer.referee_width());
    goal_width->setInt(buffer.goal_width());
    goal_depth->setInt(buffer.goal_depth());
    goal_wall_width->setInt(buffer.goal_wall_width());
    center_circle_radius->setInt(buffer.center_circle_radius());
    defense_radius->setInt(buffer.defense_radius());
    defense_stretch->setInt(buffer.defense_stretch());
    free_kick_from_defense_dist->setInt(buffer.free_kick_from_defense_dist());
    penalty_spot_from_field_line_dist->setInt(buffer.penalty_spot_from_field_line_dist());
    penalty_line_from_spot_dist->setInt(buffer.penalty_line_from_spot_dist());
    updateDerivedParameters();
  }
  #endif

  void loadDefaultsRoboCup2009() {
    line_width->setInt(FieldConstantsRoboCup2009::line_width);
    field_length->setInt(FieldConstantsRoboCup2009::field_length);
    field_width->setInt(FieldConstantsRoboCup2009::field_width);
    boundary_width->setInt(FieldConstantsRoboCup2009::boundary_width);
    referee_width->setInt(FieldConstantsRoboCup2009::referee_width);
    goal_width->setInt(FieldConstantsRoboCup2009::goal_width);
    goal_depth->setInt(FieldConstantsRoboCup2009::goal_depth);
    goal_wall_width->setInt(FieldConstantsRoboCup2009::goal_wall_width);
    center_circle_radius->setInt(FieldConstantsRoboCup2009::center_circle_radius);
    defense_radius->setInt(FieldConstantsRoboCup2009::defense_radius);
    defense_stretch->setInt(FieldConstantsRoboCup2009::defense_stretch);
    free_kick_from_defense_dist->setInt(FieldConstantsRoboCup2009::free_kick_from_defense_dist);
    penalty_spot_from_field_line_dist->setInt(FieldConstantsRoboCup2009::penalty_spot_from_field_line_dist);
    penalty_line_from_spot_dist->setInt(FieldConstantsRoboCup2009::penalty_line_from_spot_dist);
    updateDerivedParameters();
  }
  void updateDerivedParameters() {
    field_total_playable_length->setInt(field_length->getInt() + (2 * boundary_width->getInt()));
    field_total_playable_width ->setInt(field_width->getInt() + (2 * boundary_width->getInt()));
    field_total_surface_length->setInt(field_length->getInt() + (2 * (boundary_width->getInt() + referee_width->getInt())));
    field_total_surface_width->setInt(field_width->getInt() + (2 * (boundary_width->getInt() + referee_width->getInt())));
    half_field_length->setInt(field_length->getInt() / 2);
    half_field_width->setInt(field_width->getInt() / 2);
    half_line_width->setInt(line_width->getInt() / 2);
    half_goal_width->setInt(goal_width->getInt() / 2);
    half_defense_stretch->setInt(defense_stretch->getInt() / 2);
    half_field_total_playable_length->setInt(field_total_playable_length->getInt() / 2);
    half_field_total_playable_width->setInt(field_total_playable_width->getInt() / 2);
    half_field_total_surface_length->setInt(field_total_surface_length->getInt() / 2);
    half_field_total_surface_width->setInt(field_total_surface_width->getInt() / 2);
  }

  RoboCupField()
  {
    settings = new VarList("Field Configuration");
    settings->addChild(restore = new VarTrigger("Reset SSL 2009","Reset SSL 2009"));
    
    connect(restore,SIGNAL(wasEdited(VarType*)),this,SLOT(restoreRoboCup()));
    //regulation-based symmetric field:
    field_params.push_back(line_width             = new VarTypes::VarInt("Line Width"));
   
    field_params.push_back(field_length           = new VarTypes::VarInt("Field Length")); //including lines (outside to outside)
    field_params.push_back(field_width            = new VarTypes::VarInt("Field Height")); //including lines (outside to outside)
    
    field_params.push_back(boundary_width         = new VarTypes::VarInt("Boundary Width")); //width of the boundary
    field_params.push_back(referee_width          = new VarTypes::VarInt("Referee Width")); //width of the ref-walking area
    
    field_params.push_back(goal_width             = new VarTypes::VarInt("Goal Width")); //inside width of the goal
    field_params.push_back(goal_depth             = new VarTypes::VarInt("Goal Depth")); //inside depth of the goal
    field_params.push_back(goal_wall_width        = new VarTypes::VarInt("Goal Wall Width")); //goal wall thickness
    
    field_params.push_back(center_circle_radius   = new VarTypes::VarInt("Center Radius")); //radius of defense quarter circles
    
    field_params.push_back(defense_radius         = new VarTypes::VarInt("Defense Radius")); //radius of defense quarter circles
    
    //total length of the line connecting the two quarter circles of the defense area:
    field_params.push_back(defense_stretch        = new VarTypes::VarInt("Defense Stretch")); 
    
    //distance that freekickers have to be from the defense line:
    field_params.push_back(free_kick_from_defense_dist = new VarTypes::VarInt("Freekick Defense Dist")); 
    
    //distance of the penalty spot's center from the outside of the field line
    field_params.push_back(penalty_spot_from_field_line_dist = new VarTypes::VarInt("Penalty Spot Dist")); 
    
    //distance between the penalty spot and the line where all other robots must be behind during penalty
    field_params.push_back(penalty_line_from_spot_dist = new VarTypes::VarInt("Penalty Line From Spot Dist")); 
    
    //---------------------------------------------------------------
    //auto-derived variables:
    //---------------------------------------------------------------
    
    //total length of field (including boundary, BUT NOT INCLUDING REFEREE WALKING AREA)
    derived_params.push_back(field_total_playable_length        = new VarTypes::VarInt("Total Playable Length")); 
    derived_params.push_back(field_total_playable_width         = new VarTypes::VarInt("Total Playable Width")); 
    
    //total length from the outer walls (including playable boundary and referee walking area):
    derived_params.push_back(field_total_surface_length         = new VarTypes::VarInt("Total Surface Length")); 
    derived_params.push_back(field_total_surface_width          = new VarTypes::VarInt("Total Surface Width")); 
    
    derived_params.push_back(half_field_length                  = new VarTypes::VarInt("Half Field Length"));
    derived_params.push_back(half_field_width                   = new VarTypes::VarInt("Half Field Width"));
    derived_params.push_back(half_line_width                    = new VarTypes::VarInt("Half Line Width"));
    derived_params.push_back(half_goal_width                    = new VarTypes::VarInt("Half Goal Width"));
    derived_params.push_back(half_defense_stretch               = new VarTypes::VarInt("Half Defense Stretch")); 
    derived_params.push_back(half_field_total_playable_length   = new VarTypes::VarInt("Half Total Playable Length")); 
    derived_params.push_back(half_field_total_playable_width    = new VarTypes::VarInt("Half Total Playable Width")); 
    derived_params.push_back(half_field_total_surface_length    = new VarTypes::VarInt("Half Total Surface Length")); 
    derived_params.push_back(half_field_total_surface_width     = new VarTypes::VarInt("Half Total Surface Width")); 
    
    for (unsigned int i=0;i<field_params.size();i++) {
      connect(field_params[i],SIGNAL(hasChanged(VarType *)),this,SLOT(changed()));
      settings->addChild(field_params[i]);
    }
    for (unsigned int i=0;i<derived_params.size();i++) {
      derived_params[i]->addFlags( VARTYPE_FLAG_READONLY );
      settings->addChild(derived_params[i]);
    }
    
    loadDefaultsRoboCup2009();
    emit calibrationChanged();
    //setup the derived parameters to auto-update when the non-derived parameters change.
    
    
  }
  
  ~RoboCupField() {
    field_params.clear();
    derived_params.clear();
    delete line_width;
    delete field_length;
    delete field_width;
    delete boundary_width;
    delete referee_width;
    delete goal_width;
    delete goal_depth;
    delete goal_wall_width;
    delete center_circle_radius;
    delete defense_radius;
    delete free_kick_from_defense_dist;
    delete penalty_spot_from_field_line_dist;
    delete penalty_line_from_spot_dist;

    //derived:
    delete field_total_playable_length;
    delete field_total_playable_width;
    delete field_total_surface_length;
    delete field_total_surface_width;
    delete half_field_length;
    delete half_field_width;
    delete half_line_width;
    delete half_goal_width;
    delete half_defense_stretch;
    delete half_field_total_playable_length;
    delete half_field_total_playable_width;
    delete half_field_total_surface_length;
    delete half_field_total_surface_width;
    
    delete restore;
    delete settings;
  }
 public:
  signals:
  void calibrationChanged();

protected slots:
  void changed() {
    updateDerivedParameters();
    calibrationChanged();
  }
  void restoreRoboCup() {
    loadDefaultsRoboCup2009();
  }
};
    
/*!
  \class Field

  \brief Definition of point coordinates (in mm) defining one half of the field

  \author Tim Laue , (C) 2009
**/    

class RoboCupCalibrationHalfField : public QObject
{
Q_OBJECT
public:
  enum CameraPositionEnum {
    CAM_POS_HALF_NEG_X,
    CAM_POS_HALF_POS_X,
    CAM_POS_ENUM_COUNT
  };
  static CameraPositionEnum stringToCameraPositionEnum(const string & input) {
     if (input.compare("Half (Negative X)")==0) {
       return CAM_POS_HALF_NEG_X;
     } else if (input.compare("Half (Positive X)")==0) {
       return CAM_POS_HALF_POS_X;
     } else {
       return CAM_POS_HALF_NEG_X;
     }
  }
  static string cameraPositionEnumToString(const CameraPositionEnum & input) {
     if (input==CAM_POS_HALF_NEG_X) {
       return ("Half (Negative X)");
     } else if (input==CAM_POS_HALF_POS_X) {
       return ("Half (Positive X)");
     } else {
       return ("");
     }
  }
protected slots:
  void globalCalibrationChanged() {
    update();
  }
  void autoUpdateChanged() {
    if (auto_update->getBool()==true) {
      //disable all items
      for (unsigned int i = 0; i < params.size(); i++) {
        params[i]->addFlags(VARTYPE_FLAG_READONLY|VARTYPE_FLAG_NOLOAD);
      }
      camera_pos->removeFlags(VARTYPE_FLAG_READONLY);
      update();
    } else {
      //enable all items
      for (unsigned int i = 0; i < params.size(); i++) {
        params[i]->removeFlags(VARTYPE_FLAG_READONLY|VARTYPE_FLAG_NOLOAD);
      }
      camera_pos->addFlags(VARTYPE_FLAG_READONLY);
    }
  }
protected:
  RoboCupField * robocup_field;
  vector<VarType *> params;
  VarBool * auto_update;
  VarStringEnum * camera_pos;
public:
  RoboCupCalibrationHalfField(RoboCupField * _robocup_field = 0, int cam_id=0) {
    robocup_field=_robocup_field;
    if (robocup_field!=0) {
      connect(robocup_field,SIGNAL(calibrationChanged()),this,SLOT(globalCalibrationChanged()));
    }
    auto_update = new VarBool("Auto-Copy from Global Field Config",true);
    useFeaturesOnCenterCircle = new VarBool("Use features on center circle",true);
    useFeaturesInDefenseArea = new VarBool("Use features in defense area",true);
    useFeaturesInGoal = new VarBool("Use features between goal posts ",true);
    camera_pos = new VarStringEnum("Camera Position",(cam_id==0) ? cameraPositionEnumToString(CAM_POS_HALF_NEG_X) : cameraPositionEnumToString(CAM_POS_HALF_POS_X));
    camera_pos->addFlags(VARTYPE_FLAG_NOLOAD_ENUM_CHILDREN);
    for (int i=0;i<CAM_POS_ENUM_COUNT;i++) {
      camera_pos->addItem(cameraPositionEnumToString((CameraPositionEnum)i));
    }

    params.push_back(left_corner_x = new VarTypes::VarInt("left corner x", 3025));
    params.push_back(left_corner_y = new VarTypes::VarInt("left corner y", 2025));
    params.push_back(left_goal_area_x = new VarTypes::VarInt("left goal area x", 3025)); 
    params.push_back(left_goal_area_y = new VarTypes::VarInt("left goal area y", 675)); 
    params.push_back(left_goal_post_x = new VarTypes::VarInt("left post area x", 3025)); 
    params.push_back(left_goal_post_y = new VarTypes::VarInt("left post area y", 350)); 
    params.push_back(right_goal_post_x = new VarTypes::VarInt("right post area x", 3025)); 
    params.push_back(right_goal_post_y = new VarTypes::VarInt("right post area y", -350)); 
    params.push_back(right_goal_area_x = new VarTypes::VarInt("right goal area x", 3025)); 
    params.push_back(right_goal_area_y = new VarTypes::VarInt("right goal area y", -675)); 
    params.push_back(right_corner_x = new VarTypes::VarInt("right corner x", 3025));
    params.push_back(right_corner_y = new VarTypes::VarInt("right corner y", -2025));
    params.push_back(left_centerline_x = new VarTypes::VarInt("left centerline x", 0));
    params.push_back(left_centerline_y = new VarTypes::VarInt("left centerline y", 2025));
    params.push_back(left_centercircle_x = new VarTypes::VarInt("left centercircle x", 0)); 
    params.push_back(left_centercircle_y = new VarTypes::VarInt("left centercircle y", 500)); 
    params.push_back(centerpoint_x = new VarTypes::VarInt("centercircle x", 0)); 
    params.push_back(centerpoint_y = new VarTypes::VarInt("centercircle y", 0)); 
    params.push_back(right_centercircle_x = new VarTypes::VarInt("right centercircle x", 0)); 
    params.push_back(right_centercircle_y = new VarTypes::VarInt("right centercircle y", -500)); 
    params.push_back(right_centerline_x = new VarTypes::VarInt("right centerline x", 0));
    params.push_back(right_centerline_y = new VarTypes::VarInt("right centerline y", -2025));
    params.push_back(centercircle_radius = new VarTypes::VarInt("centercircle radius",500));
    params.push_back(defense_area_radius = new VarTypes::VarInt("defense area radius",500));
    params.push_back(defense_stretch = new VarTypes::VarInt("defense stretch",350));
    
    
    connect(auto_update,SIGNAL(hasChanged(VarType *)),this,SLOT(autoUpdateChanged()));
    connect(camera_pos,SIGNAL(hasChanged(VarType *)),this,SLOT(globalCalibrationChanged()));

    autoUpdateChanged();
    update();
  }

  bool isCamPosHalfNegX() const { return (stringToCameraPositionEnum(camera_pos->getSelection()) == CAM_POS_HALF_NEG_X); }

  void update() {
    if (auto_update->getBool() == true && robocup_field!=0) copyFromRoboCupField(robocup_field,stringToCameraPositionEnum(camera_pos->getSelection()));
  }

  void copyFromRoboCupField(RoboCupField * field, CameraPositionEnum pos) {
    int mult_x=1;
    int mult_y=1;
    if (pos==CAM_POS_HALF_NEG_X) {
      mult_x=-1;
      mult_y=-1;
    } else if (pos==CAM_POS_HALF_POS_X) {
      //mult_y=-1;
    }
    left_corner_x->setInt(mult_x*(field->half_field_length->getInt()));
    left_corner_y->setInt(mult_y*(field->half_field_width->getInt()));
    left_goal_area_x->setInt(mult_x*(field->half_field_length->getInt()));
    left_goal_area_y->setInt(mult_y*(field->defense_radius->getInt()+field->half_defense_stretch->getInt()));
    left_goal_post_x->setInt(mult_x*(field->half_field_length->getInt()));
    left_goal_post_y->setInt(mult_y*(field->half_goal_width->getInt()));

    right_corner_x->setInt(left_corner_x->getInt());
    right_corner_y->setInt(-left_corner_y->getInt());
    right_goal_area_x->setInt(left_goal_area_x->getInt());
    right_goal_area_y->setInt(-left_goal_area_y->getInt());
    right_goal_post_x->setInt(left_goal_post_x->getInt());
    right_goal_post_y->setInt(-left_goal_post_y->getInt());

    left_centerline_x->setInt(0); 
    left_centerline_y->setInt(left_corner_y->getInt());
    left_centercircle_x->setInt(0);
    left_centercircle_y->setInt(mult_y*(field->center_circle_radius->getInt()));

    right_centerline_x->setInt(0);
    right_centerline_y->setInt(-left_centerline_y->getInt());
    right_centercircle_x->setInt(0);
    right_centercircle_y->setInt(-left_centercircle_y->getInt());

    centerpoint_x->setInt(0);
    centerpoint_y->setInt(0);
    
    centercircle_radius->setInt(field->center_circle_radius->getInt());
    defense_area_radius->setInt(field->defense_radius->getInt());
    defense_stretch->setInt(field->defense_stretch->getInt());
  }

  ~RoboCupCalibrationHalfField()
  {
    params.clear();
    delete auto_update;
    delete useFeaturesOnCenterCircle;
    delete useFeaturesInDefenseArea;
    delete useFeaturesInGoal;
    delete camera_pos;
    delete left_corner_x;
    delete left_corner_y;
    delete left_goal_area_x; 
    delete left_goal_area_y; 
    delete left_goal_post_x; 
    delete left_goal_post_y; 
    delete right_goal_post_x; 
    delete right_goal_post_y; 
    delete right_goal_area_x; 
    delete right_goal_area_y; 
    delete right_corner_x;
    delete right_corner_y;
    delete left_centerline_x;
    delete left_centerline_y;
    delete left_centercircle_x;
    delete left_centercircle_y; 
    delete centerpoint_x; 
    delete centerpoint_y; 
    delete right_centercircle_x; 
    delete right_centercircle_y; 
    delete right_centerline_x;
    delete right_centerline_y;
    delete centercircle_radius;
    delete defense_area_radius;
    delete defense_stretch;
  }
  
  void addSettingsToList(VarList& list) 
  {
    list.addChild(auto_update);
    list.addChild(camera_pos);
    list.addChild(useFeaturesOnCenterCircle);
    list.addChild(useFeaturesInDefenseArea);
    list.addChild(useFeaturesInGoal);
    list.addChild(left_corner_x);
    list.addChild(left_corner_y);
    list.addChild(left_goal_area_x); 
    list.addChild(left_goal_area_y); 
    list.addChild(left_goal_post_x); 
    list.addChild(left_goal_post_y); 
    list.addChild(right_goal_post_x); 
    list.addChild(right_goal_post_y); 
    list.addChild(right_goal_area_x); 
    list.addChild(right_goal_area_y); 
    list.addChild(right_corner_x);
    list.addChild(right_corner_y);
    list.addChild(left_centerline_x);
    list.addChild(left_centerline_y);
    list.addChild(left_centercircle_x); 
    list.addChild(left_centercircle_y); 
    list.addChild(centerpoint_x); 
    list.addChild(centerpoint_y); 
    list.addChild(right_centercircle_x); 
    list.addChild(right_centercircle_y); 
    list.addChild(right_centerline_x);
    list.addChild(right_centerline_y);
    list.addChild(centercircle_radius);
    list.addChild(defense_area_radius);
    list.addChild(defense_stretch);
  }
  
  VarTypes::VarBool* useFeaturesOnCenterCircle;
  VarTypes::VarBool* useFeaturesInDefenseArea;
  VarTypes::VarBool* useFeaturesInGoal;
  VarTypes::VarInt* left_corner_x;
  VarTypes::VarInt* left_corner_y;
  VarTypes::VarInt* left_goal_area_x; 
  VarTypes::VarInt* left_goal_area_y; 
  VarTypes::VarInt* left_goal_post_x; 
  VarTypes::VarInt* left_goal_post_y; 
  VarTypes::VarInt* right_goal_post_x; 
  VarTypes::VarInt* right_goal_post_y; 
  VarTypes::VarInt* right_goal_area_x; 
  VarTypes::VarInt* right_goal_area_y; 
  VarTypes::VarInt* right_corner_x;
  VarTypes::VarInt* right_corner_y;
  VarTypes::VarInt* left_centerline_x;
  VarTypes::VarInt* left_centerline_y;
  VarTypes::VarInt* left_centercircle_x; 
  VarTypes::VarInt* left_centercircle_y; 
  VarTypes::VarInt* centerpoint_x; 
  VarTypes::VarInt* centerpoint_y; 
  VarTypes::VarInt* right_centercircle_x; 
  VarTypes::VarInt* right_centercircle_y; 
  VarTypes::VarInt* right_centerline_x;
  VarTypes::VarInt* right_centerline_y;
  VarTypes::VarInt* centercircle_radius;
  VarTypes::VarInt* defense_area_radius;
  VarTypes::VarInt* defense_stretch;
};


#endif // FIELD_H
