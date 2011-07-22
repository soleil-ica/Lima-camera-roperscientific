#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Exceptions.h"

#include "RoperScientificCamera.h"

using namespace lima;
using namespace lima::RoperScientific;


Camera::CameraThread::CameraThread(Camera& cam)
	: m_cam(&cam)
{
	m_cam->m_acq_frame_nb = 0;
	m_force_stop = false;
}

void Camera::CameraThread::start()
{
  CmdThread::start();
  waitStatus(Ready);
  
}

void Camera::CameraThread::init()
{
	setStatus(Ready);
}

void Camera::CameraThread::execCmd(int cmd)
{
  int status = getStatus();
  switch (cmd) {
  case StartAcq:
    if (status != Ready)
      throw LIMA_HW_EXC(InvalidValue,  "Not Ready to StartAcq");
    execStartAcq();
    break;
  }
}




void Camera::CameraThread::execStartAcq()
{

  int16 status;
  uns32 not_needed;
  int acq_frame_nb;
  void *f_address;

	
  setStatus(Exposure);
  
  StdBufferCbMgr& buffer_mgr = m_cam->m_buffer_ctrl_mgr.getBuffer();
  buffer_mgr.setStartTimestamp(Timestamp::now());
  
  int nb_frames = m_cam->m_nb_frames;
  int& frame_nb = m_cam->m_acq_frame_nb;


  m_cam->m_acq_frame_nb = 0;
  
  acq_frame_nb = 0;

  while( nb_frames ) {
    
    if(m_force_stop){
	m_force_stop = false;
	setStatus(Ready);
	goto stop;
    }
    
    if( !m_cam->cam_sim_mode ){
    
      if(m_cam->m_int_acq_mode == 0){
	pl_exp_start_seq(m_cam->m_handle, m_cam->m_frame );
	
	/* wait for data or error */
	while( pl_exp_check_status( m_cam->m_handle, &status, &not_needed ) && 
	       (status != READOUT_COMPLETE && status != READOUT_FAILED) ){
	  usleep(9000);
	}
      
	/* Check Error Codes */
	if( status == READOUT_FAILED ) {    
	  m_cam->get_camera_error();
	  throw LIMA_HW_EXC(Error, "Data collection error");
	  break;
	}
      } else if (m_cam->m_int_acq_mode == 1){
	
	while( pl_exp_check_cont_status( m_cam->m_handle, &status, &not_needed, &not_needed ) && 
	       (status != READOUT_COMPLETE && status != READOUT_FAILED) ){
	  usleep(9000);
	}
      
	/* Check Error Codes */
	if( status == READOUT_FAILED ) {   
	  m_cam->get_camera_error();
	  throw LIMA_HW_EXC(Error, "Data collection error");
	  break;
	}
	if ( pl_exp_get_oldest_frame( m_cam->m_handle, &f_address )) {
	  m_cam->m_frame = (uns16*)f_address;
	}
      } else if (m_cam->m_int_acq_mode == 2){
	
	while( pl_exp_check_cont_status( m_cam->m_handle, &status, &not_needed, &not_needed ) && 
	       (status != READOUT_COMPLETE && status != READOUT_FAILED) ){
	  usleep(9000);
	}
      
	/* Check Error Codes */
	if( status == READOUT_FAILED ) {   
	  m_cam->get_camera_error();
	  throw LIMA_HW_EXC(Error, "Data collection error");
	  break;
	}
	if ( pl_exp_get_latest_frame(  m_cam->m_handle, &f_address )) {
	   m_cam->m_frame = (uns16*)f_address;
	}
      }
     
    }else{
      double req_time;
      req_time = m_cam->m_exposure;
      if (req_time > 0) {
	usleep(long(req_time * 1e6));
      }
      for(int i = 0; i < m_cam->m_size/sizeof(uns16); i++){
	m_cam->m_frame[i]= i;
      }
    }

 
    setStatus(Readout);
    

     unsigned short *ptr = (unsigned short*)buffer_mgr.getFrameBufferPtr(frame_nb);
     memcpy(ptr,m_cam->m_frame,m_cam->m_size/sizeof(uns16)+1);
    


    buffer_mgr.setStartTimestamp(Timestamp::now());

    HwFrameInfoType frame_info;
    frame_info.acq_frame_nb = acq_frame_nb;
    buffer_mgr.newFrameReady(frame_info); 
   
    m_cam->m_acq_frame_nb = acq_frame_nb;

    acq_frame_nb++; 
    nb_frames--;


  } /* End while */

 stop:
  if(!m_cam->cam_sim_mode){   

    if(m_cam->m_int_acq_mode == 0){
      /* Finish the sequence */
      
      pl_exp_finish_seq(m_cam->m_handle, m_cam->m_frame, 0);
    }  else {

      /* Stop the acquisition */
      
      pl_exp_stop_cont(m_cam->m_handle,CCS_HALT);

      /* Finish the sequence */
      pl_exp_finish_seq(m_cam->m_handle, m_cam->m_pr_buffer, 0); 
      //      if( m_cam->m_pr_buffer ) free( m_cam->m_pr_buffer );
    
    }
  
    /*Uninit the sequence */
    
    pl_exp_uninit_seq();
    
  }
  
  if( m_cam->m_frame ) free( m_cam->m_frame );


  setStatus(Ready);
}

int Camera::CameraThread::getNbAcquiredFrames()
{
  return m_cam->m_acq_frame_nb;
}



Camera::Camera(int camNum) :
  m_thread(*this)
	      //  , m_buffer_cb_mgr(m_buffer_alloc_mgr)
	      //  , m_buffer_ctrl_mgr(m_buffer_cb_mgr)
{
  DEB_CONSTRUCTOR();

  int16 param;
  float tmp_temp;


  cam_sim_mode = 1;

  if(! cam_sim_mode ){

    /* Open pvcam library */
    pl_pvcam_uninit();
    
    if( !pl_pvcam_init() ) {
      throw LIMA_HW_EXC(Error, "could not initialize pvcam lib");
    } 
    
    /* Get camera name */
    
    if( !pl_cam_get_name( camNum, m_camera_name ) ) {
      get_camera_error(); 
      throw LIMA_HW_EXC(Error, "failed to get Roper Scientific camera name");
    }
    /* Establish comunication with the camera */
    
    int err;
    if( err = pl_cam_open(m_camera_name, &m_handle, OPEN_EXCLUSIVE ) ) {
      printf( "camera %s open\n");
    } else { 
      get_camera_error();
      throw LIMA_HW_EXC(Error, "failed to open camera");
    }
    /* Read current temperature and set the temperature set point */
    
    pl_get_param( m_handle, PARAM_TEMP, ATTR_CURRENT, (void *)&param );
    tmp_temp = param/100; 
    
    if( (tmp_temp < 10) ){
      param = (int16) (-40.*100);
    } else {
      param = (int16) (25.*100);
    }
    
    pl_set_param(m_handle, PARAM_TEMP_SETPOINT, (void *)&param );  
    
    get_camera_error();
  } else {
    strcpy(m_camera_name, "Simu RoperScientific");
  }


  m_camera_busy = 0;

  m_thread.start();


}

Camera::~Camera()
{
  DEB_DESTRUCTOR();
  
  if(! cam_sim_mode ){
    pl_cam_close( m_handle );
    
    pl_pvcam_uninit();
  }

  /*
  if(m_cam_connected)
    {
      PvCommandRun(m_handle,"AcquisitionStop");
      PvCaptureEnd(m_handle);
      PvCameraClose(m_handle);
    }
  PvUnInitialize();

  */
  //  if(m_frame[0].ImageBuffer)
  //   free(m_frame[0].ImageBuffer);
  //  if(m_frame[1].ImageBuffer)
  //   free(m_frame[1].ImageBuffer);
}


HwBufferCtrlObj* Camera::getBufferMgr()
{
  DEB_MEMBER_FUNCT();

  return &m_buffer_ctrl_mgr;
}

Camera::Status Camera::getStatus()
{
  DEB_MEMBER_FUNCT();

  int thread_status = m_thread.getStatus();

  DEB_RETURN() << DEB_VAR1(thread_status);
  
  switch (thread_status) {
  case CameraThread::Ready:
    return Camera::Ready;
  case CameraThread::Exposure:
    return Camera::Exposure;
  case CameraThread::Readout:
    return Camera::Readout;
  case CameraThread::Latency:
    return Camera::Latency;
  default:
    throw LIMA_HW_EXC(Error, "Invalid thread status");
  }
}


void Camera::setNbFrames(int nb_frames)
{
	if (nb_frames < 0)
		throw LIMA_HW_EXC(InvalidValue, "Invalid nb of frames");

	m_nb_frames = nb_frames;
}

void Camera::getNbFrames(int& nb_frames)
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(m_nb_frames);
  
  nb_frames = m_nb_frames;
}

/** @brief test if the camera is monochrome
 */
bool Camera::isMonochrome() const
{
  DEB_MEMBER_FUNCT();

  return !strcmp(m_sensor_type,"Mono");
}

VideoMode Camera::getVideoMode() const
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(m_video_mode);

  return m_video_mode;
}

void Camera::getCameraName(std::string& name)
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(m_camera_name);

  name = m_camera_name;
}
void Camera::setVideoMode(VideoMode aMode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(aMode);

  /*
  ImageType anImageType;
  tPvErr error;
  switch(aMode)
    {
    case Y8:
      error = PvAttrEnumSet(m_handle, "PixelFormat", "Mono8");
      anImageType = Bpp8;
      break;
    case Y16:
      error = PvAttrEnumSet(m_handle, "PixelFormat", "Mono16");
      anImageType = Bpp16;
      break;
    case BAYER_RG8:
      error = PvAttrEnumSet(m_handle, "PixelFormat", "Bayer8");
      anImageType = Bpp8;
      break;
    case BAYER_RG16:
      error = PvAttrEnumSet(m_handle, "PixelFormat", "Bayer16");
      anImageType = Bpp16;
      break;
    default:
      throw LIMA_HW_EXC(InvalidValue,"This video mode is not managed!");
    }
  
  if(error)
    throw LIMA_HW_EXC(Error,"Can't change video mode");
  */
  //  m_video_mode = aMode;
  //  maxImageSizeChanged(Size(m_maxwidth,m_maxheight),anImageType);
}


void Camera::prepareAcq()
{
  DEB_MEMBER_FUNCT();


  rgn_type region={ 0, 511, 1, 0, 511, 1};
  
  if(m_use_full_frame){
    setFullFrame(&region);
  } else {
    setROI(&region);
  }

  /* Init a sequence  */

  if(! cam_sim_mode ){
    if( pl_exp_init_seq() ) {
      printf( "experiment sequence initialized\n" );
    } else {
      get_camera_error();
      throw LIMA_HW_EXC(Error, "sequence initialization failed");
    }
  
    
    if(m_int_acq_mode == 0){
      /* Set the region, exposure mode and exposure time.
	 Second argument (exp_total) -> Number of images to take.
	 size is the returned number of bytes in the pixel stream */
      
      if( pl_exp_setup_seq( m_handle, 1, 1, &region,m_trigger_mode, (long)m_exposure, &m_size ) ) {
	printf( "setup sequence OK\n" );
	printf( "frame size = %i\n", m_size );
      } else {
	get_camera_error();
	throw LIMA_HW_EXC(Error, "sequence setup failed");
      }
    } else if(m_int_acq_mode == 1){
      if( pl_exp_setup_cont( m_handle, 1, &region, m_trigger_mode, (long)m_exposure, &m_size, CIRC_NO_OVERWRITE ) ) {
      } else {
	get_camera_error();
	throw LIMA_HW_EXC(Error, "continuous setup failed");
      }
    } else if (m_int_acq_mode == 2){
      if( pl_exp_setup_cont( m_handle, 1, &region, m_trigger_mode, (long)m_exposure, &m_size, CIRC_OVERWRITE ) ) {
      } else {
	get_camera_error();
	throw LIMA_HW_EXC(Error, "continuous setup failed");
      }
    }	

  } else {
    //    size = 250;
    m_size = 1024*1024;
  }
  
  if(m_int_acq_mode != 0){
    m_pr_buffer = (unsigned short*)malloc( m_size * 2 * 3 ); // set up a circular buffer of 3 frames
    pl_exp_start_cont(m_handle, m_pr_buffer, m_size );

  }
   
  m_frame = (unsigned short*)malloc( m_size * 2); // Size of memory in bytes
  
  if( !m_frame ) {
    get_camera_error();
    throw LIMA_HW_EXC(Error, "memory allocation error");
  } else {
    printf( "frame of data at address %x\n", m_frame );
  }


}

/** @brief start the acquisition.
    must have m_video != NULL and previously call _allocBuffer
*/
void Camera::startAcq()
{
  DEB_MEMBER_FUNCT();
  
  m_thread.m_force_stop = false;
  m_continue_acq = true;
  m_acq_frame_nb = 0;

  m_camera_busy = 1;


  int16 status;
  uns32 not_needed;

  FILE *data;
  struct timeval  tp1;
  int16 param;

  m_thread.sendCmd(CameraThread::StartAcq);
  m_thread.waitNotStatus(CameraThread::Ready);



}


void   Camera::stopAcq(){

  DEB_MEMBER_FUNCT();
  
  // if(!cam_sim_mode){   
    /* Finish the sequence */
  
  //    pl_exp_finish_seq( m_handle, m_frame, 0);
    
    /*Uninit the sequence */
    
  //    pl_exp_uninit_seq();
    
  //  }

  m_thread.m_force_stop = true;

/*
  if( m_frame ) free( m_frame );
*/
}

void Camera::reset()
{
  DEB_MEMBER_FUNCT();
  //@todo maybe something to do!
}


//-----------------------------------------------------
//
//-----------------------------------------------------
double Camera::exposure() const
{
  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(m_exposure);
  //	AutoMutex aLock(m_cond.mutex());
  return m_exposure;
}


void Camera::setExposure(double val)
{

  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(val);

  m_exposure = val;

}

long Camera::getGain()
{
  
  DEB_MEMBER_FUNCT();

  int16 param;

  if(!cam_sim_mode){
    if(!m_camera_busy){
      pl_get_param( m_handle, PARAM_GAIN_INDEX, ATTR_CURRENT, (void *)&param );
    }
  } else {
    param = 1;
  }

  DEB_RETURN() << DEB_VAR1(param);
  return param;
}

void Camera::setGain(long gain)
{

  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(gain);

  int16 param;
  
  param = (int16)gain;

  if(!cam_sim_mode){
    if(!m_camera_busy){
      pl_set_param(m_handle, PARAM_GAIN_INDEX, (void *)&param ); 
    }
  }
}

void Camera::get_camera_error()
{
 
  DEB_MEMBER_FUNCT();

  int16 i;
    
  //  char msg[200];
  
  if(!m_camera_busy){ 
    i = pl_error_code();
    m_error_code = i;
    //   pl_error_message( i, msg );

    //    pl_error_message( i, m_error_msg );
  }
}      

void Camera::setFullFrame( rgn_type* roi ) {
  uns16 param; 

  DEB_MEMBER_FUNCT();

  roi->s1 = 0;
  pl_get_param( m_handle, PARAM_SER_SIZE, ATTR_DEFAULT, (void *)&param );
  roi->s2 = param-1;
  roi->sbin = 1;
  roi->p1 = 0;
  pl_get_param( m_handle, PARAM_PAR_SIZE, ATTR_DEFAULT, (void *)&param );
  roi->p2 = param-1;
  roi->pbin = 1;
}
  
void Camera::setROI( rgn_type* roi) { 
  DEB_MEMBER_FUNCT();

  roi->s1 = m_roi_s1;
  roi->s2 = m_roi_s2;
  roi->sbin = m_roi_sbin;
  roi->p1 = m_roi_p1;
  roi->p2 = m_roi_p2;
  roi->pbin = m_roi_pbin;
}


void Camera::setTrigMode(int mode)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);
  
  m_trigger_mode = mode;

}

int Camera::getTrigMode()
{
  DEB_MEMBER_FUNCT();  
  DEB_RETURN() << DEB_VAR1(m_trigger_mode);

  return m_trigger_mode;

}

float  Camera::getTemperature()
{
  DEB_MEMBER_FUNCT();

  int16 param;
  float temp;

  if(!cam_sim_mode){
    if(!m_camera_busy){
      pl_get_param(m_handle, PARAM_TEMP, ATTR_CURRENT, (void *)&param );
      temp = param/100; 
    }
  } else {
    temp = 25.;
  }
  
  DEB_RETURN() << DEB_VAR1(temp);

  return temp;

}

float  Camera::getTemperatureSetPoint()
{
  DEB_MEMBER_FUNCT();

  int16 param;
  float temp;

  if(!cam_sim_mode){
    if(!m_camera_busy){
      pl_get_param(m_handle, PARAM_TEMP_SETPOINT, ATTR_CURRENT, (void *)&param);
      temp = param/100; 
    }
  } else {
    temp = 25.;
  }
  
  DEB_RETURN() << DEB_VAR1(temp);

  return temp;

}

void  Camera::setTemperatureSetPoint(float temp)
{  

  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(temp);

  int16 param;

  if(!cam_sim_mode){
    if(!m_camera_busy){
      param = (int16) (temp*100);
	
      pl_set_param(m_handle, PARAM_TEMP_SETPOINT, (void *)&param );  
    }
  }
}


std::string Camera::getInternalAcqMode(){

  DEB_MEMBER_FUNCT();

  if(m_int_acq_mode == 0){
    DEB_RETURN() << DEB_VAR1("STANDARD");
    return "STANDARD";
  } else if(m_int_acq_mode == 1){
    DEB_RETURN() << DEB_VAR1("CONTINUOUS");
    return "CONTINUOUS";
  } else if(m_int_acq_mode == 2){
    DEB_RETURN() << DEB_VAR1("FOCUS");
    return "FOCUS";
  }
}

void  Camera::setInternalAcqMode(std::string mode){

  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(mode);

  if(mode.compare("STANDARD") == 0){
    m_int_acq_mode = 0;
  } else if (mode.compare("CONTINUOUS") == 0){
    m_int_acq_mode = 1;
  } else if (mode.compare("FOCUS") == 0){
    m_int_acq_mode = 2;
  } else {
    m_int_acq_mode = 0;
  } 
}
    
uns16 Camera::getMaxWidth(){
 
  uns16   width;
  
  DEB_MEMBER_FUNCT();
  if(!cam_sim_mode){  
    pl_get_param( m_handle, PARAM_SER_SIZE, ATTR_DEFAULT, (void *)&width ); 
  } else {
    width = 1024;
  }

  DEB_RETURN() << DEB_VAR1(width);
    
  m_max_width = width;

  return width;

}

uns16 Camera::getMaxHeight(){
 
  uns16   height;

  DEB_MEMBER_FUNCT();
  if(!cam_sim_mode){  
    pl_get_param( m_handle, PARAM_PAR_SIZE, ATTR_DEFAULT, (void *)&height ); 
  } else {
    height = 1024;
  }

  DEB_RETURN() << DEB_VAR1(height);
      
  m_max_height = height;
  
  return height;

}

int  Camera::getUseFullFrame(){

  DEB_MEMBER_FUNCT();
  DEB_RETURN() << DEB_VAR1(m_use_full_frame);

  return m_use_full_frame;

}

void Camera::setUseFullFrame(int val){

  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(val);

  m_use_full_frame = val;

}


void Camera::setBin(const Bin& bin)
{
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(bin);

  m_roi_sbin = bin.getX();
  m_roi_pbin = bin.getY();

}

void Camera::getBin(Bin& bin)
{

  DEB_MEMBER_FUNCT();

  Bin tmp_bin(m_roi_sbin,m_roi_pbin);

  bin = tmp_bin;
 
  DEB_RETURN() << DEB_VAR1(bin);
}

void Camera::checkBin(Bin& bin)
{
  
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(bin);
  printf("Teresa: Camera::checkBin \n");
}

void Camera::getRoi( Roi &roi ) const
{

  DEB_MEMBER_FUNCT();

  Point point1(m_roi_s1,m_roi_p1);
  Point point2(m_roi_s2,m_roi_p2);
  Roi tmp_roi(point1,point2);

  roi = tmp_roi;

  DEB_RETURN() << DEB_VAR1(roi);
}

void Camera::setRoi( const Roi &roi )
{  
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR1(roi);

  
  Point tmp_top = roi.getTopLeft();

  m_roi_s1 = tmp_top.x;
  m_roi_p1 = tmp_top.y;

  Point tmp_bottom = roi.getBottomRight();

  m_roi_s2 = tmp_bottom.x;
  m_roi_p2 = tmp_bottom.y;
 
}

void Camera::checkRoi( const Roi &set_roi,Roi &hw_roi ) const
{
  
  DEB_MEMBER_FUNCT();
  DEB_PARAM() << DEB_VAR2(set_roi, hw_roi);
  if(set_roi.getSize().getWidth() > m_max_width){
    throw LIMA_HW_EXC(Error, "roi outside of camera limits");
  } else if(set_roi.getSize().getHeight() > m_max_height){
    throw LIMA_HW_EXC(Error, "roi outside of camera limits");
  } else {
    hw_roi = set_roi;
  }
}
