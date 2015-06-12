namespace RoperScientific
{
  class Camera
  {
%TypeHeaderCode
#include <RoperScientificCamera.h>
%End
  public:
    Camera(int);
    ~Camera();
      
    int getNbAcquiredFrames() const;

    void setNbFrames(int );
    void getNbFrames(int&);
	      
    void 	startAcq();	
    void 	stopAcq();
    void        prepareAcq();
    void	reset();

    void      setGain(long);
    long      getGain();

    void      setSpeedTableIndex(unsigned);
    unsigned getSpeedTableIndex();
    const std::string& getADCRate();

    float     getTemperature(); 
    float     getTemperatureSetPoint();
    void      setTemperatureSetPoint( float);

    std::string  getInternalAcqMode();
    void         setInternalAcqMode(std::string );
	
  };
};
