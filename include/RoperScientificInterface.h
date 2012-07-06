#ifndef ROPERSCIENTIFICINTERFACE_H
#define ROPERSCIENTIFICINTERFACE_H

#include "HwInterface.h"
#include "HwBufferMgr.h"
#include "Debug.h"
#include "RoperScientificCamera.h"

using namespace std;

namespace lima
{
namespace RoperScientific
{
class Interface;

/*******************************************************************
 * \class DetInfoCtrlObj
 * \brief Control object providing RoperScientific detector info interface
 *******************************************************************/

class DetInfoCtrlObj: public HwDetInfoCtrlObj
{
DEB_CLASS_NAMESPC(DebModCamera, "DetInfoCtrlObj", "RoperScientific");

public:
	DetInfoCtrlObj(Camera& cam);
	virtual ~DetInfoCtrlObj();

	virtual void getMaxImageSize(Size& max_image_size);
	virtual void getDetectorImageSize(Size& det_image_size);

	virtual void getDefImageType(ImageType& def_image_type);
	virtual void getCurrImageType(ImageType& curr_image_type);
	virtual void setCurrImageType(ImageType curr_image_type);

	virtual void getPixelSize(double& x_size, double &y_size);
	virtual void getDetectorType(std::string& det_type);
	virtual void getDetectorModel(std::string& det_model);

	virtual void registerMaxImageSizeCallback(HwMaxImageSizeCallback& cb);
	virtual void unregisterMaxImageSizeCallback(HwMaxImageSizeCallback& cb);

private:
	Camera& m_cam;
};

/*******************************************************************
 * \class SyncCtrlObj
 * \brief Control object providing RoperScientific synchronization interface
 *******************************************************************/

class SyncCtrlObj: public HwSyncCtrlObj
{
DEB_CLASS_NAMESPC(DebModCamera, "SyncCtrlObj", "RoperScientific");

public:
	SyncCtrlObj(Camera& cam, DetInfoCtrlObj& det);
	virtual ~SyncCtrlObj();

	virtual bool checkTrigMode(TrigMode trig_mode);
	virtual void setTrigMode(TrigMode trig_mode);
	virtual void getTrigMode(TrigMode& trig_mode);

	virtual void setExpTime(double exp_time);
	virtual void getExpTime(double& exp_time);

	virtual void setLatTime(double lat_time);
	virtual void getLatTime(double& lat_time);

	virtual void setNbHwFrames(int nb_frames);
	virtual void getNbHwFrames(int& nb_frames);

	virtual void getValidRanges(ValidRangesType& valid_ranges);

private:
	Camera& m_cam;
};

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

/*******************************************************************
 * \class Interface
 * \brief RoperScientific hardware interface
 *******************************************************************/

class Interface: public HwInterface
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
	virtual int getNbAcquiredFrames();
	virtual int getNbHwAcquiredFrames();
	
    //! get the camera object to access it directly from client
    Camera& getCamera() { return m_cam;}

private:
	Camera& m_cam;
	CapList m_cap_list;
	DetInfoCtrlObj m_det_info;
	SyncCtrlObj m_sync;
	BinCtrlObj m_bin;
	RoiCtrlObj m_roi;
};

} // namespace RoperScientific
} // namespace lima

#endif // ROPERSCIENTIFICINTERFACE_H
