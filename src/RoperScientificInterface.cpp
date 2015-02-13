
#include "RoperScientificCamera.h"
#include "RoperScientificDetInfoCtrlObj.h"
#include "RoperScientificSyncCtrlObj.h"
#include "RoperScientificBinCtrlObj.h"
#include "RoperScientificRoiCtrlObj.h"
#include "RoperScientificInterface.h"


using namespace lima;
using namespace lima::RoperScientific;
using namespace std;



/*******************************************************************
 * \brief Hw Interface constructor
 *******************************************************************/

Interface::Interface(Camera& cam) :
		m_cam(cam)
{
	DEB_CONSTRUCTOR();
	m_det_info = new DetInfoCtrlObj(cam);
	m_sync = new SyncCtrlObj(cam);
	m_roi = new RoiCtrlObj(cam);
	m_bin = new BinCtrlObj(cam);
	HwBufferCtrlObj *buffer = cam.getBufferCtrlObj();

	m_cap_list.push_back(HwCap(m_det_info));
	m_cap_list.push_back(HwCap(buffer));
	m_cap_list.push_back(HwCap(m_sync));
	m_cap_list.push_back(HwCap(m_roi));
	m_cap_list.push_back(HwCap(m_bin));

}

//-----------------------------------------------------
//
//-----------------------------------------------------
Interface::~Interface()
{
	DEB_DESTRUCTOR();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::getCapList(HwInterface::CapList &cap_list) const
{
	DEB_MEMBER_FUNCT();
	cap_list = m_cap_list;
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::reset(ResetLevel reset_level)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(reset_level);

	stopAcq();

	Size image_size;
	m_det_info->getMaxImageSize(image_size);
	ImageType image_type;
	m_det_info->getDefImageType(image_type);
	FrameDim frame_dim(image_size, image_type);

	HwBufferCtrlObj *buffer = m_cam.getBufferCtrlObj();
	buffer->setFrameDim(frame_dim);

	buffer->setNbConcatFrames(1);
	buffer->setNbBuffers(1);
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::prepareAcq()
{
	DEB_MEMBER_FUNCT();

	m_cam.prepareAcq();

}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::startAcq()
{
	DEB_MEMBER_FUNCT();
	m_cam.startAcq();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::stopAcq()
{
	DEB_MEMBER_FUNCT();
	m_cam.stopAcq();
}

//-----------------------------------------------------
//
//-----------------------------------------------------
void Interface::getStatus(StatusType& status)
{

	Camera::Status camera_status = m_cam.getStatus();
	switch (camera_status)
    {
    case Camera::Ready:
      status.set(HwInterface::StatusType::Ready);
      break;
    case Camera::Exposure:
      status.set(HwInterface::StatusType::Exposure);
      break;
    case Camera::Readout:
      status.set(HwInterface::StatusType::Readout);
      break;
    case Camera::Latency:
      status.set(HwInterface::StatusType::Latency);
      break;
    case Camera::Fault:
      status.set(HwInterface::StatusType::Fault);
    }
}

//-----------------------------------------------------
//
//-----------------------------------------------------
int Interface::getNbHwAcquiredFrames()
{
	DEB_MEMBER_FUNCT();
	int aNbAcquiredFrames;

	aNbAcquiredFrames = m_cam.getNbAcquiredFrames();

	DEB_RETURN() << DEB_VAR1(aNbAcquiredFrames);
	return aNbAcquiredFrames;
}
//-----------------------------------------------------
