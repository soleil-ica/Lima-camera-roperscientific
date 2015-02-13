#ifndef ROPERSCIENTIFICINTERFACE_H
#define ROPERSCIENTIFICINTERFACE_H

#include "lima/Debug.h"
#include "RoperScientificCompatibility.h"
#include "lima/HwInterface.h"
#include "lima/HwBufferMgr.h"


using namespace std;

namespace lima
{
namespace RoperScientific
{
class Camera;
class DetInfoCtrlObj;
class SyncCtrlObj;
class BinCtrlObj;
class RoiCtrlObj;
/*******************************************************************
 * \class Interface
 * \brief RoperScientific hardware interface
 *******************************************************************/

class LIBROPERSCIENTIFIC_API Interface: public HwInterface
{
DEB_CLASS_NAMESPC(DebModCamera, "RoperScientificInterface", "RoperScientific");

public:
	Interface(Camera& cam);
	virtual ~Interface();

	//- From HwInterface
	virtual void getCapList(CapList&) const;
	virtual void reset(ResetLevel reset_level);
	virtual void prepareAcq();
	virtual void startAcq();
	virtual void stopAcq();
	virtual void getStatus(StatusType& status);
	virtual int getNbHwAcquiredFrames();
	
    //! get the camera object to access it directly from client
    Camera& getCamera() { return m_cam;}

private:
	Camera& m_cam;
	CapList m_cap_list;
	DetInfoCtrlObj* m_det_info;
	SyncCtrlObj* m_sync;
	BinCtrlObj* m_bin;
	RoiCtrlObj* m_roi;
};

} // namespace RoperScientific
} // namespace lima

#endif // ROPERSCIENTIFICINTERFACE_H
