#ifndef ROPERSCIENTIFICBINCTRLOBJ_H
#define ROPERSCIENTIFICBINCTRLOBJ_H

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
     * \class BinCtrlObj
     * \brief Control object providing Basler Bin interface
     *******************************************************************/
    class BinCtrlObj : public HwBinCtrlObj
    {
    	DEB_CLASS_NAMESPC(DebModCamera, "BinCtrlObj", "RoperScientific"); 
    public:
    	BinCtrlObj(Camera& cam);
    	virtual ~BinCtrlObj();

    	virtual void setBin(const Bin& bin);
    	virtual void getBin(Bin& bin);
    	//allow all binning
    	virtual void checkBin(Bin& bin);
    private:
    	Camera& m_cam;

    };

  } // namespace RoperScientific
} // namespace lima

#endif // ROPERSCIENTIFICBINCTRLOBJ_H
