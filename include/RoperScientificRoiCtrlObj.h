#ifndef ROPERSCIENTIFICROICTRLOBJ_H
#define ROPERSCIENTIFICROICTRLOBJ_H

#include "RoperScientific.h"

#include "HwInterface.h"
#include "Debug.h"
#include "RoperScientificCamera.h"

namespace lima
{
  namespace RoperScientific
  {
    class Camera;

    /*******************************************************************
     * \class RoiCtrlObj
     * \brief Control object providing Basler Roi interface
     *******************************************************************/

    class RoiCtrlObj : public HwRoiCtrlObj
    {
    	DEB_CLASS_NAMESPC(DebModCamera, "RoiCtrlObj", "RoperScientific");

    public:
    	RoiCtrlObj(Camera& cam);
    	virtual ~RoiCtrlObj();

    	virtual void setRoi(const Roi& set_roi);
    	virtual void getRoi(Roi& hw_roi);
    	virtual void checkRoi(const Roi& set_roi, Roi& hw_roi);

    private:
    	Camera& m_cam;
    };

  } // namespace RoperScientific
} // namespace lima

#endif // ROPERSCIENTIFICROICTRLOBJ_H
