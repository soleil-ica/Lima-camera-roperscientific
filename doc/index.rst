.. _camera-roperscientific:

RoperScientific / Princeton
---------------------------

.. image:: Roper-Scientific.jpg

.. image:: PIMAX4_camera_front_back_500.png

Introduction
````````````

This plugin control a RoperScientific/Princeton camera under Windows and Linux, using the PVCAM (Photometrics Virtual Camera Access Method) libraries.

It is in production at SOLEIL under windows and it has been tested at Desy under Linux.
Model used at SOLEIL: PI-MTE:2048B

Prerequisite
````````````
The RoperScientific is connected to a specific computer with a PCI board. The Lima/RoperScientific client must run on this PC.

Initialisation and Capabilities
```````````````````````````````

Implementing a new plugin for new detector is driven by the LIMA framework but the developer has some freedoms to choose which standard and specific features will be made available. This section is supposed to give you the correct information regarding how the camera is exported within the LIMA framework.

Camera initialisation
......................

The camera will be initialized within the :cpp::class:`RoperScientific::Camera` object. The camera number (as an integer) should be given to the constructor. For example: 0.

Std capabilites
................

This plugin has been implemented in respect of the mandatory capabilites but with some limitations according
to some programmer's  choices.  We only provide here extra information for a better understanding
of the capabilities for the RoperScientific camera.

* HwDetInfo

 - Max image size is : 2048 * 2048
 - 16 bit unsigned type is supported

* HwSync

  Trigger type supported are:

	- IntTrig
	- ExtTrigSingle
	- ExtTrigMult
	- ExtGate


Optional capabilites
........................

* HwBin:

	- all values are accepted

* HwRoi

Specific control parameters
.............................

Some specific paramaters are available within the camera hardware interface. Those parameters should be used carefully and one should refer to the camera SDK (or user's guide) documentation for a better understanding.

* getTemperature()

* set/getTemperatureSetPoint()

* set/getGain()

* set/getInternalAcqMode()

 - "FOCUS"
 - "STANDARD"

* set/getSpeedTableIndex()

Configuration
`````````````

No Specific hardware configuration are needed

How to use
``````````

Here is the list of accessible fonctions to configure and use the RoperScientific detector:

.. code-block:: cpp

	void setGain(long);
	long getGain();

	void setFullFrame(rgn_type* roi);
	void setBinRoiParameters(rgn_type* roi);

	void setSpeedTableIndex(unsigned);
	unsigned getSpeedTableIndex(void);
	const std::string& getADCRate(void);

	double 	getTemperature();
	double 	getTemperatureSetPoint();
	void	setTemperatureSetPoint(double temperature);


Code example in python:

.. code-block:: python

  from Lima import RoperScientific
  from lima import Core

  cam = RoperScientific.Camera(0)

  hwint = RoperScientific.Interface(cam)
  ct = Core.CtControl(hwint)

  acq = ct.acquisition()

  # set some configuration
  cam.setTemperatureSetPoint(0)
  cam.setAdcRate(0) # 0-1MHz, 1-100KHz


  # setting new file parameters and autosaving mode
  saving=ct.saving()

  pars=saving.getParameters()
  pars.directory='/buffer/lcb18012/opisg/test_lima'
  pars.prefix='test1_'
  pars.suffix='.edf'
  pars.fileFormat=Core.CtSaving.EDF
  pars.savingMode=Core.CtSaving.AutoFrame
  saving.setParameters(pars)

  # now ask for 2 sec. exposure and 10 frames
  acq.setAcqExpoTime(2)
  acq.setNbImages(10)

  ct.prepareAcq()
  ct.startAcq()

  # wait for last image (#9) ready
  lastimg = ct.getStatus().ImageCounters.LastImageReady
  while lastimg !=9:
    time.sleep(0.1)
    lastimg = ct.getStatus().ImageCounters.LastImageReady

  # read the first image
  im0 = ct.ReadImage(0)
