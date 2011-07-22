#ifndef ROPERSCIENTIFICCAMERA_H
#define ROPERSCIENTIFICCAMERA_H
#include "RoperScientific.h"
#include "Debug.h"
#include "Constants.h"
#include "HwMaxImageSizeCallback.h"
#include "HwBufferMgr.h"
#include "ThreadUtils.h"

namespace lima
{
  namespace RoperScientific
  {
    class Camera : public HwMaxImageSizeCallbackGen   
    //    class Camera 
    {
      friend class Interface;
      DEB_CLASS_NAMESPC(DebModCamera,"Camera","RoperScientific");


    public:
      
      enum Status {
	Ready, Exposure, Readout, Latency,
      };

      Camera(int camNum = 0);
      ~Camera();
     
      double exposure() const;
      void setExposure(double expo);

      bool isMonochrome() const;
	
      Status getStatus();
      int    getNbAcquiredFrames() const {return m_acq_frame_nb;}

      VideoMode getVideoMode() const;
      void 	setVideoMode(VideoMode);
      
      void	getCameraName(std::string& name);
	
      void 	startAcq();	
      void 	stopAcq();
      void      prepareAcq();
      void	reset();

      void setNbFrames(int  nb_frames);
      void getNbFrames(int& nb_frames);

      void      setGain(long);
      long      getGain();

    
      void      get_camera_error(void);

      void      setFullFrame( rgn_type* roi );  
      void      setROI( rgn_type* roi); 

      int       cam_sim_mode;

      HwBufferCtrlObj* getBufferMgr();

      float     getTemperature();
      float     getTemperatureSetPoint();
      void      setTemperatureSetPoint(float temp);

      int       getTrigMode();
      void      setTrigMode(int mode);

      std::string  getInternalAcqMode();
      void         setInternalAcqMode(std::string mode);

      uns16       getMaxWidth();
      uns16       getMaxHeight();

      int       getUseFullFrame();
      void      setUseFullFrame(int val);

      
      void setBin(const Bin& bin);
      void getBin(Bin& bin);
      void checkBin(Bin& bin);

      void getRoi( Roi &roi ) const;
      void setRoi( const Roi &roi );
      void checkRoi( const Roi& set_roi, Roi& hw_roi) const;

    private:
      class CameraThread : public CmdThread{
      public:
	enum { // Status
	  Ready = MaxThreadStatus, Exposure, Readout, Latency,
	};
	
	enum { // Cmd 
	  StartAcq = MaxThreadCmd, StopAcq,
	};
	
	CameraThread(Camera& cam);
	
	virtual void start();
	
	int getNbAcquiredFrames();
	bool m_force_stop;
	
      protected:
	virtual void init();
	virtual void execCmd(int cmd);
      private:
	void execStartAcq();
	Camera* m_cam;

      };
      friend class CameraThread;

      void 		_allocBuffer();

      SoftBufferCtrlMgr m_buffer_ctrl_mgr;


      bool 		m_cam_connected;
      int short     	m_handle; /* Camera handle */
      char		m_camera_name[128];
      char		m_sensor_type[64];
      int		m_maxwidth, m_maxheight;
      
      unsigned short         *m_frame;
      unsigned short         *m_pr_buffer;
      

      VideoMode		m_video_mode;
      int		m_acq_frame_nb;
      bool		m_continue_acq;
      double 		m_exposure; 

      char              m_error_msg[200];
      int               m_camera_busy;
      int               m_error_code;
      int               m_nb_frames;

      ushort m_roi_s1;
      ushort m_roi_s2;
      ushort m_roi_sbin;
      ushort m_roi_p1;
      ushort m_roi_p2;
      ushort m_roi_pbin;

      int    m_use_full_frame;
      
      uns32  m_size;

      int m_trigger_mode;
      int m_int_acq_mode;
 
      uns16 m_max_width;
      uns16 m_max_height;

      CameraThread m_thread;
     

    };
  }
}
#endif
