#ifndef OPENTISSUE_TRACKBALL_BELL_H
#define OPENTISSUE_TRACKBALL_BELL_H
//
// OpenTissue, A toolbox for physical based simulation and animation.
// Copyright (C) 2007 Department of Computer Science, University of Copenhagen
//
#include <OpenTissue/configuration.h>

#include <OpenTissue/trackball/trackball_generic.h>

namespace OpenTissue
{

  template<typename real_type_ = double>
  class Bell : public GenericTrackball<real_type_>
  {
  public:

    typedef GenericTrackball<real_type_>             base_type;
    typedef typename base_type::real_type            real_type;
    typedef typename base_type::vector3_type         vector3_type;
    typedef typename base_type::matrix3x3_type       matrix3x3_type;
    typedef typename base_type::quaternion_type      quaternion_type;
    typedef typename base_type::gl_transform_type    gl_transform_type;

  public:

    Bell() 
      : base_type()
    {
      reset();
    }

    Bell(real_type const & radius) 
      : base_type(radius)
    {
      reset();
    }

    void reset()
    {
      base_type::reset();
      project_onto_surface(this->m_anchor_position);
      project_onto_surface(this->m_current_position);
    }

    void begin_drag(real_type const & x, real_type const & y)
    {
      this->m_angle = 0.0;
      this->m_axis.clear();

      this->m_xform_anchor = this->m_xform_current;
      this->m_xform_incremental = diag(1.0);
      this->m_xform_current = diag(1.0);

      this->m_anchor_position = vector3_type(x,y,0);
      project_onto_surface(this->m_anchor_position);
      this->m_current_position = vector3_type(x,y,0);
      project_onto_surface(this->m_current_position);
    }

    void drag(real_type const & x, real_type const & y)
    {
      this->m_current_position = vector3_type(x,y,0);
      project_onto_surface(this->m_current_position);
      compute_incremental(this->m_anchor_position,this->m_current_position,this->m_xform_incremental);
    }

    void end_drag(real_type const & x, real_type const & y)
    {
      this->m_current_position = vector3_type(x,y,0);
      project_onto_surface(this->m_current_position);
      compute_incremental(this->m_anchor_position,this->m_current_position,this->m_xform_incremental);
    }

  private:

    void project_onto_surface(vector3_type & P)
    {
      using std::sqrt;
      const static real_type radius2 = this->m_radius * this->m_radius;
      real_type length2 = P(0)*P(0) + P(1)*P(1);

      if (length2 <= radius2 / 2.0)
        P(2) = sqrt(radius2 - length2);
      else
      {
        P(2) = radius2 / (2.0 * sqrt(length2));
        real_type length = sqrt(length2 + P[2] * P[2]);
        P /= length;
      }
    }

    void compute_incremental(vector3_type const & anchor, vector3_type const & current, matrix3x3_type & transform)
    {
      quaternion_type  Q_anchor ( 0, unit(anchor)  );
      quaternion_type  Q_current( 0, unit(current) );
      quaternion_type Q_rot = -prod(Q_current,Q_anchor);
      this->m_axis = Q_rot.v();
      this->m_angle = atan2(length(this->m_axis), Q_rot.s());
      transform = Q_rot;  //--- KE 20060307: Kaiip extracted axis and angle from this conversion!!!
    }
  };
} // namespace OpenTissue

// OPENTISSUE_TRACKBALL_BELL_H
#endif
