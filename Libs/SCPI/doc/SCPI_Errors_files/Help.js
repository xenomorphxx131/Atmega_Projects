//Javascript file for PNAHelp
//Rev 01312006

//THe following is for sending feedback from the skin button. 

function mail()
{
var subj="mailto:pna_help@am.exch.agilent.com?subject=Please include the topic title!"
document.location.href=subj
}


// Below is for swapping out buttons

function Btnout(x)
{var im=eval("'" + pth + "Button.gif'")
x.src=im}

function Btnovr(x)
{var im=eval("'" + pth + "ButtonO.gif'")
x.src=im}

function Btndwn(x)
{var im=eval("'" + pth + "ButtonD.gif'")
x.src=im}


// Below is for Return to top
var y1,x1,s,btm



function handleError() {
	return true;
}



//Onscroll, figure out where the corner is
function scrolit(x1,y1)
{s=Math.round(getScrolled());
btm=s+y1;
window.onerror = handleError;
var obj=document.all.image.style;
shiftTo(obj,x1,btm)}

//How far has it scrolled?
function getScrolled()
{return document.body.scrollTop}

//Keep it in the corner
function shiftTo(obj,x1,y1)
{obj.pixelLeft=x1
obj.pixelTop=y1}

//calc new corner after load and resize, 

function calcCorner()
{x1=Math.round(getInsideWindowWidth()-40);
y1=Math.round(getInsideWindowHeight()-30);}

//calc window size
function getInsideWindowWidth()
{return document.body.clientWidth}

function getInsideWindowHeight()
{return document.body.clientHeight}

//Onclick, goto Top
function scrolToTop()
{scrollTo(0,0)}

//Swap the Arrow
function Tpout(x)
{var im=eval("'" + pth + "Tp.gif'")
x.src=im}

function Tpovr(x)
{var im=eval("'" + pth + "TpO.gif'")
x.src=im}

function Tpdwn(x)
{var im=eval("'" + pth + "TpD.gif'")
x.src=im}


//Below is for operating the Tabs

function tabclick(x)
{
//Immediately scroll to Top
scrollTo(0,0)

//loop to pull all tabs in and hide text
for (var i=0;i<n;i++)
{var tab=eval("tab" + i)
tab.className="tab-in"
var toxt=eval("txt"+i+".style")
toxt.display="none"}

//pull out one and display text
var toxt=eval("txt"+x+".style")
toxt.display=""
var tab=eval("tab" + x)
tab.className="tab-out"}

function tbovr(x)
//only turn text red if the tab is "in"
{if(x.className=="tab-in")
{x.className="tab-ovr"}}

function tbout(x)
{if(x.className=="tab-ovr")
{x.className="tab-in"}}

//Below is for linking to a bookmark within the same tabbed topic 
//display all the text - then go the bookmark (argument)

function goThere(bm,x)
{
//call standard tabclick to expose the correct tab
tabclick(x)

//Go to the bookmark
var bookmrk=eval("'"+bm+"'")
window.location.hash=bookmrk
}


//Below is for swapping out right and left arrows (part of tabs)

function Rout(x)
{var im=eval("'" + pth + "Right.gif'")
x.src=im}

function Rovr(x)
{var im=eval("'" + pth + "Rovr.gif'")
x.src=im}

function Rdwn(x)
{var im=eval("'" + pth + "Rdwn.gif'")
x.src=im}

function Lout(x)
{var im=eval("'" + pth + "Left.gif'")
x.src=im}

function Lovr(x)
{var im=eval("'" + pth + "Lovr.gif'")
x.src=im;}

function Ldwn(x)
{var im=eval("'" + pth + "Ldwn.gif'")
x.src=im}

//Below is for drop-down text
//Pass the id name of the text

function drop(x)

{
if (x.style.display=="none")
   {x.style.display=""}
else
   {x.style.display="none"}
}

//Below is definitions for glossary terms that appear in text-only popups

One_port_calibration="Corrects a test set for port 1 or port 2 directivity, frequency response, and source match errors. The process requires three known standard terminations, for example, open, short, and load. Also known as 3-term error correction."
One_port_device="A device with a single connector or path to the device's circuitry. Examples include an oscillator and a load."
Two_port_device="A device with two connectors or other paths to the device's circuitry. Examples include filters, SAW devices, attenuators, matching pads, and amplifiers."
Active_Channel="The highlighted channel affected by front panel functions."
Active_marker="The marker on a trace that can be repositioned either by front panel controls or by programming commands."
Active_trace="The trace that is being swept (updated) with incoming signal information."
ADC="Analog To Digital Converter"
Address="The identification (represented by a name, label, or number) for a register, location in storage, or any other data source or destination. Examples are the location of a station in a communications network, or a device on the HP-IB."
Admittance_Y="The inverse of an impedance (i.e. the ratio of current to voltage). Complex admittances take the form Y = G + jB(t)"
ALC="Automatic Level Control."
AM_group_delay="A technique for the measurement of group delay through a device which utilizes an amplitude modulated (AM) source. Note: The actual delay of the modulation envelope is measured directly with an external scalar detector. Devices that distort the amplitude of a signal cannot be measured. These include amplifiers with automatic gain control (AGC) and devices subject to saturation or power limiting."
AM="Amplitude Modulation  The process, or result of the process, of varying the amplitude of a carrier signal. The resulting modulated carrier contains information that can be recovered by demodulation."
Analog="The general class of devices or circuits in which the output varies as a continuous function of the input."
Annotation="The labeling of specific information on the screen(such as frequency or power)."
Annual_Calibration="The process of periodically (usually annually) verifying an instrument is performing to specifications. A calibration certificate is awarded after verification."
Aperture="The frequency span of the network analyzer used for calculating group delay. The narrower the aperture, the finer the resolution of the group delay variations, but noise is reduced by increasing the aperture."
Array_="A set of numbers or characters that represents any given function."
ASCII="American Standard Code for Information Interchange."
Attenuation="Denotes a reduction in signal amplitude. The difference between transmitted and received power due to loss through equipment, lines, or other transmission devices; usually expressed in decibels."
Attenuator="An RF or microwave device used to reduce the power level of a signal by precise, incremental amounts over its entire frequency range."
Automatic_Gain_Control="(AGC) A circuit used in amplifiers and other active devices to keep its RF power level constant as other parameters change, such as frequency. Synonym: Automatic Leveling Control (ALC)"
Autoscale="An analyzer feature that evaluates waveforms and adjusts controls to stable and enhance the display."
AUX="Auxiliary; refers to rear-panel input connector."
Averaging="A noise reduction technique that computes each data point based on consecutive sweeps and weighted by a user-specified averaging factor. Each new sweep is averaged into the trace until the total number of sweeps is equal to the averaging factor."
B_R="The ratio of data sampled at B to the data sampled at R."
Band_Pass="A range of frequencies that are passed through a device, such as a filter. Frequencies not within the band pass are limited or attenuated."
Bandwidth_Limit="The condition prevailing when the system bandwidth is exceeded and signal distortion occurs beyond specifications."
Binary="A method of representing numbers in a scale of two (on or off, high-level or low-level, one or zero). A compact, fast format used to transfer information to and from the analyzer."
Broadband_device="A device that operates over a very wide frequency range and exhibits only small variations in response over that range."
Buffer="A storage device used when transmitting information to compensate for a difference in the rate of flow of information between two devices (such as the analyzer and controller)."
BW="Bandwidth  The difference between the frequencies of a continuous frequency band within which performance of a device falls within specifications."
Byte="Eight bits of data representing one character processed as a unit."
Calibration="A process that removes or reduces systematic (repeatable) measurement errors by measuring known standards from a calibration kit. Also know as Error correction."
Cal_interpolation="A user selectable network analyzer feature that calculates (interpolates) new error correction terms from existing terms when there is a change in network analyzer parameters, such as IF bandwidth, power, or sweep time. The resulting error correction is not as accurate as completing a full 2-port calibration."
Cal_kit="Hardware and software required to perform error correction on a network analyzer for a specific measurement and/or test set."
Cal_type="A Calibration type describes the algorithm used to correct the measurement.  CalTypes supported by the PNA, range from a full THREE PORT (27 term) calibration, to simple single term response calibrations."
Characteristic_impedance="The impedance looking into the end of an infinitely long lossless transmission line."
Command="A set of instructions that are translated into instrument actions. The actions are usually made up of individual steps that together can execute an operation."
Continuous_sweep="The analyzer condition where traces are automatically updated each time trigger conditions are met."
Controller="An external computer connected to an instrument to control its operation."
Corrected="Measurements made after performing error correction."
Corrected_Data="Complex data with correction data applied. If correction is NOT on, this buffer is identical to Raw Data."
CPU="Central Processing Unit."
Crosstalk="The occurrence of a signal at one port of a device being affected by a signal in any other path. Isolation is the measurement of crosstalk."
Cutoff_frequency="In filters, the frequency at which attenuation is 3dB below the band pass signal level, known as the 3dB points."
CW="Continuous wave: A single frequency (rather than a swept frequency)."
DAC="Digital to analog converter"
Data_format="The ways in which measurement data can be presented graphically. On a network analyzer, the choices are Cartesian/rectilinear (XY plot with log or linear magnitude, phase, group delay, SWR, real and imaginary, and dBV, dBmV and dBuV), polar (magnitude and angle), magnitude and phase, and Smith chart. Not all display modes are available on all network analyzers. In addition, displays can present this information in various combinations of traces. Common modes are dual, (the ability to display more than one trace, usually over the same frequency range), and alternate, (the ability to display more than one trace, each with different frequency range and type)."
Data_points="The total number of measurement points made in a single measurement. The points can be in units of frequency, power, or time. The number of points often dictates measurement speed, resolution, and aperture."
dB="Decibel: a relative unit of measure. The ratio in dB is given by: where P 1 and P 2 are the measured powers. The dB is preferred instead of arithmetic ratios or percentages because when components are connected in series, their effect on power, expressed in dB, may be arithmetically added and subtracted. For example, if a 3dB attenuator is connected to a 10dB amplifier, the net gain of the two components is (-3dB + 10dB = +7dB)"
dBm="Absolute unit of measure in decibels: 0dBm = 1 mW. The conventions of the dB (adding and subtracting) continue to apply."
DC="Direct Current"
Default="A known set of conditions used in the absence of user-defined conditions."
Demodulation="The process of recovering from a modulated carrier, information in the form of a signal having essentially the same characteristics as the original modulating signal; accomplished by signal detection."
Detection="The process of demodulating signal carriers. There are two basic ways of providing signal detection in network analyzers: Diode detectors (used in broadband applications) and heterodyning, (used in narrowband applications)."
Detector_diode="A device used to convert a RF signal to a proportional DC level. If the signal is amplitude modulated, the diode strips the RF carrier signal from the modulation. Many sources used with scalar analyzers are amplitude modulated with a 27.778 kHz signal and then detected in the network analyzer. Phase information on the signal carrier is lost in diode detection."
Deviation_from_linear_phase="Linear phase refers to the nature of the phase shift of a signal through a device. The phase is linear if a plot of phase shift versus frequency is a straight line using analog scales. Deviation from linear phase causes signal distortion."
Digital_demodulation="A technique of extracting the information used to modulate a signal. Digital signal processing algorithms are used on the signal after it has been converted from an analog to a digital form (digitized)."
Digital="Pertaining to the class of devices or circuits in which the output varies in discrete steps."
Dimension="To specify the size of an array. The number of array rows or columns. Also know as declaring an array"
Directional_coupler="A 3-port device typically used for separately sampling the backward (reflected) wave in a transmission line."
Directivity="In a 3-port directional coupler, the ratio of the power present at the auxiliary port when the signal is traveling in the forward direction to the power present at the auxiliary port when the same signal is traveling in the reverse direction."
Disk="A circular, magnetic storage medium."
Display_dynamic_accuracy="The amplitude uncertainty, usually in dB, over the display dynamic range."
Display_dynamic_range="The amplitude range, in dB, over which the display dynamic accuracy applies."
Display_phase_dynamic_accuracy="The phase measurement uncertainty, usually in degrees, for measurements whose units are in degrees."
Display="Verb: To show annotation and measurement data on the CRT. Noun: The physical surface of the CRT screen."
Distortion="Deterioration of a signal's quality due to the nonlinear characteristics of a device or system transfer function. Distortion is measured as a combination of the changes in amplitude, frequency and phase of signal at the output of a device or system as compared to the signal at the input."
Drift="The slow change in signal frequency."
DUT="Device Under Test."
Dynamic_range="The range of signal levels, from minimum to maximum, that can be reliably measured simultaneously. Dynamic range allows small signals to be measured in the presence of large signals. Source power and receiver compression usually limits the maximum boundary to dynamic range. Receiver residual responses and noise floor usually limit the minimum power boundary."
Ecal="Electronic calibration: A calibration system for electronic calibration of RF and microwave vector network analyzers. The electronic calibration system creates a twelve-term, two-port error model and then provides a confidence check of the calibration. The Ecal system consists of a repeatable, variable-impedance, solid-state calibration standard and a mainframe control unit which interfaces with various network analyzers."
Electrical_delay="A simulated variable length of lossless transmission line, added to or subtracted from a receiver input, to compensate for interconnecting cables. The firmware equivalent of mechanical or analog 'line stretchers' in other network analyzers."
EMI="Electro-Magnetic Interference: Unintentional interfering signals generated within or external to electronic equipment. Typical sources could be power-line transients, noise from switching-type power supplies and/or spurious radiation from oscillators. EMI is suppressed with power-line filtering, shielding, etc."
Error_correction="A process that removes or reduces systematic (repeatable) measurement errors by measuring known standards from a calibration kit. Also know as Measurement Calibration and Error correction"
Error_message="A message on a display that indicates an error condition. Missing or failed hardware, improper user operation, or other conditions that require additional attention can cause an error condition. Generally, the requested action or operation cannot be completed until the condition is resolved."
Error_Terms="Data that is calculated from the standards data using the algorithm supplied by the CalType (error correction model)."
ESD="Electro Static Discharge"
External_trigger_signal="A TTL signal that is input to an analyzer and initiates a measurement sweep or similar event, making the measurements synchronous with the external triggering source."
Filter="A passive device that allows some frequencies to pass and attenuates others, depending on the type and specifications. A high-pass filter passes frequencies above the cutoff frequency, a low-pass filter passes frequencies below the cutoff frequency, and a band-pass filter passes frequencies between two specific frequencies."
Firmware="An assembly made up of hardware and instruction code. The hardware and instruction code is integrated and forms a functional set that cannot be altered during normal operation. The instruction code, permanently installed in the circuitry of the instrument, is classified as ROM (read only memory). The firmware determines the operating characteristics of the instrument or equipment."
Flatness="The amplitude and phase response of a device under test (DUT), a signal source, a receiver, or a combination of these. See frequency response"
FM="Frequency modulation"
Frequency_accuracy="The uncertainty with which the frequency of a signal or spectral component is indicated, either in an absolute sense or relative to another signal or spectral component. Absolute and relative frequency accuracies are specified independently."
Frequency_range="The range of frequencies over which a device or instrument performance is specified. The maximum frequency range of many microwave spectrum analyzers can be extended with the application of external mixers."
Frequency_resolution="The ability of an analyzer to separate closely spaced frequency signals and display them individually."
Frequency_response="The peak-to-peak variation in the displayed signal amplitude over a specified center frequency range. Frequency response is typically specified in terms of dB, relative to the value midway between the extremes. It also may be specified relative to the calibrator signal."
Frequency_span="The magnitude of the displayed frequency component. Span is represented by the horizontal axis of the display. Generally, frequency span is given as the total span across the full display. Some analyzers represent frequency span (scan width) as a per-division value."
Frequency_stability="The ability of a frequency component to remain unchanged in frequency or amplitude over short and long-term periods of time."
Frequency="The number of periodic oscillations, vibrations, or waves per unit of time, usually expressed in cycles per second, or Hertz (Hz)."
Front_panel_key="Keys that are located on the front panel of an instrument. The key labels identify the function the key activities. Numeric keys and step keys are two examples of front panel keys."
Full_two_port_calibration="Correction of six systematic errors: Directivity, Source match, Load match, Reflection tracking, Transmission tracking, and Isolation. To accurately measure all four S-parameters of a two-port device, these six error terms must be characterized in the forward and reverse directions, making a total of 12 terms. The user usually has the option of omitting isolation from the correction process. Synonyms: Full 2-port error correction, SOLT calibration, Measurement Calibration"
Fundamental_frequency="In any waveform, the lowest frequency component; all other components are harmonics. A pure sinusoid has only one component, the fundamental frequency."
GHz="Gigahertz"
GIF="Graphics Interchange Format - Standard graphic format to store bitmapped graphics files."
Giga="Prefix for one billion"
GPI0="General Purpose Input / Output; a connector usually on the back of an instrument that allows communication with other test equipment, external test sets, switches, and computers that enable the instrument to be triggered or to trigger external equipment. An example is a foot switch that continues or cycles a measurement, allowing the operator to use both hands on the test hardware."
GPIB="General Purpose Interface Bus - IEEE 488 bus is interconnect bus and protocol, allows linking of instruments and computer."
Graticule="(or Grid) Enclosed area where waveform is displayed on instrument. Tick marks, on frame or axis, are a scaling aid for making visual measurements."
Group_delay="A measure of the transit time of a signal through a DUT versus frequency. Group delay can be calculated by differentiating the DUT's insertion-phase response versus frequency."
Hardcopy="Paper copy of data."
Host_computer="A computer or device on a network that provides end users with services such as computation and database access and that usually performs network control functions."
Impedance="The ratio of voltage to current at a port of a circuit, expressed in ohms."
Initialize="The process that assigns information locations to a disk to prepare the magnetic media to accept files."
Input_attenuator="An attenuator between the input connector and the first mixer of a spectrum analyzer (also called an RF attenuator). The input attenuator is used to adjust the signal level incident to the first mixer, and to prevent gain compression due to high-level or broadband signals. It is also used to set the dynamic range by controlling the degree of internally-generated distortion. For some spectrum analyzers, changing the input attenuator settings changes the vertical position of the signal on the display, which then changes the reference level accordingly. In Hewlett-Packard microprocessor-controlled spectrum analyzers, the IF gain is changed to compensate for changes in input attenuator settings. Because of this, the signals remain stationary on the display, and the reference level is not changed."
Input="A path intended for putting a signal into an instrument.Most network analyzers have either 3 (labeled A, B, and R) or 4 inputs (labeled A, B, R1, and R2). Inputs are not the same as channels."
Insertion_Loss="The difference between the power measured before and after the insertion of a device. The attenuation between the input and output of a device."
Interface="A connection that allows a common communication link between two or more devices."
Intermodulation_distortion ="Undesired frequency components resulting from the interaction of two or more spectral components passing through a device having nonlinear behavior, such as a mixer or an amplifier. The undesired components are related to the fundamental components by sums and differences of the fundamentals and various harmonics. The algorithm is: f1 ±f2, 2xf1 ±f2, 2xf2±f1, 3xf1 ±2x f2, and so on"
Interpolate="To determine a value of a signal between to adjacent points by a procedure or algorithm."
IP_address="Internet protocol address: a unique number that is assigned to each device which is to be connected to a TCP/IP network. Before using an analyzer on a network, your network administrator will need to assign an IP address. An IP address consists of a 32-bit value presented in decimal dot notation: 4 octets (bytes) separated by a dot."
IP="Internet Protocol"
ISDN="Integrated Services Digital Network: A standard digital service capability that features one or more circuit-switched communication channels capable of carrying digital voice, data, or image signals, a packet-switched channel for out-of-band signaling and control. In addition, ISDN provides a collection of standard and optional features that support information productivity for the user providing higher-speed Internet access than analog systems."
Isolation="A specification or measure of the immunity that one signal has to being effected by another adjacent signal. The occurrence is known as crosstalk."
Isolator="An RF device used for providing isolation between paths and components. Made from 3-port circulator, the third port being terminated in a 50ohm load."
KB="Kilobyte"
Kbs="Kilobytes per second"
Kilo="Prefix for one thousand"
LAN="Local Area Network"
LCD="Liquid Crystal Display"
LED="Light Emitting Diode"
Limit_line_file="The user-memory file that contains the limit-line table entries."
Limit_line_table="The line segments of a limit line are stored in the limit-line table. The table can be recalled to edit the line segments, then restored in the limit-line file."
Limit_lines="Lines input by the user that overlay the analyzer's measurement data to allow automatic detection of data that is out of the acceptable range. Pass/Fail annotation, audio alarms, or electronic output can be triggered to notify the operator or on-line computer program of the over-limit condition."
Linear_Device="A device in which the output is continuously proportional to the input."
LO_feedthrough="The response that in a superheterodyne system when the first local oscillator frequency is equal to the first IF. In spectrum analyzers, the LO feedthrough is a 0 Hz marker with no error, so it can be used to improve the frequency accuracy of spectrum analyzers with nonsynthesized LO systems."
LO="Local Oscillator. In a superheterodyne system, the LO is mixed with the received signal to produce a sum or difference equal to the intermediate frequency (IF) of the receiver."
Load_match="A measure of how close the device's terminating load impedance is to the ideal transmission line impedance. Match is usually measured as return loss or standing wave ratio (SWR) of the load."
Load="A one port microwave device used to terminate a path in its characteristic impedance."
Local_Lock_Out="A condition or command that prevents analyzer front-panel entries (and disables the Local key)."
Local_Operation="To operate manually from the front panel."
Log_display="The display mode in which vertical deflection is a logarithmic function of the input signal voltage. Log display is also called logarithmic display. The display calibration is set by selecting the value of the top graticule line (reference level), and scale factor in volts per division."
Log="Logarithm."
Magnitude="The amplitude of a signal measured as a voltage in its characteristic impedance without regard to phase. Synonym: Scalar."
Marker_functions="Mathematical or statistical computation on the data of one or more markers to provide the operator more information. For example, the marker delta function calculates and displays the difference between two markers."
Marker="A graphical symbol along a display trace that is annotated with measurement characteristics of that specific data point."
Maximum_input_level="The maximum signal power that may be safely applied to the input of an analyzer. The maximum input level is typically 1 W (-30 dBm) for Hewlett-Packard spectrum analyzers."
MB="Megabyte"
Meas_Result="The same as the displayed trace, unless you request data in a different format."
Measurement_uncertainty="The quantified amount of error in a measurement situation. Error correction is intended to reduce the amount of uncertainty. The following are sources of measurement errors that lead to uncertainty:* Systematic errors (imperfections in calibration standards, connectors, cables, and instrumentation) * Random errors (noise, connector repeatability) * Drift (source and instrumentation)"
Mega="Prefix for one million"
Memory="A storage medium, device, or recording medium into which data can be stored and held until some later time, and from which the entire original data may be retrieved."
Memory_Result="Like naMeasResult, this buffer is the same as the displayed memory trace, unless you request the data in a different format."
Menu="The analyzer functions that appear on the display. These selections may evoke a series of other related functions that establish groups called menus."
MHz="Megahertz"
milli="Prefix for one-thousandth"
Modem="Modulator/Demodulator"
Modulation="The process, or the result of the process, of varying a characteristic of a carrier signal with an information-bearing signal, causing the carrier to contain the information. See AM and FM"
Monitor="Any external display."
ms="Millisecond: one thousandth of a second"
mW="Milliwatt: one thousandth of a watt"
Narrowband_device="A device whose transfer characteristics are intended to operate over a very narrow frequency range and are designed to provide well-defined amplitude responses in that range, such as a band pass filter."
Narrowband="The frequency resolution of the analyzer's receiver that is sufficiently narrow to resolve the magnitude and phase characteristics of narrowband devices. The reduced receiver bandwidth usually decreases the noise floor of the receiver, providing more measurement amplitude range."
Network_analysis="The characterization of a device, circuit, or system derived by comparing a signal input going into the device to a signal or signals coming out from the device."
NIST="National Institute of Standards and Technology"
Noise_figure="(NF): For a two-port device, a measure of how the noise generated inside the device degrades the signal-to-noise ratio of a signal passing through the device at 290 degrees, usually expressed in dB."
Noise_floor="The analyzer's internal displayed noise. The noise level often limits how small a signal magnitude can be measured."
Noise="Random variations of unwanted or disturbing energy in a communications system from man-made and natural sources that affects or distorts the information carried by the signal. See also signal-to-noise ratio."
Nonvolatile_memory="Memory data that is retained in the absence of an ac power source. This memory is typically retained with a battery."
Normalize="To subtract one trace from another to eliminate calibration data errors or to obtain relative information."
Offset="To move or set off a determined amount. Used in instruments for offsetting frequencies, limits, delay, loss, impedance, and so forth"
Output_attenuation="The ability to attenuate the signal from the source in order to control its power level."
Parser="Reads program messages from the input queue of a device in the order they were received from the controller. The parser determines what actions the analyzer should take. One of the most important functions of the command parser is to determine the position of a program message in the analyzer SCPI command tree. When the command parser is reset, the next element it receives is expected to arise from the base of the analyzer command tree."
PC="Personal Computer"
PDF="Portable Document Format (used on the Web)"
Peak_search="A function on an analyzer that searches for the largest signal and places a marker on it."
Phase="The fractional part of a cycle through which an oscillation has advanced, measured from an arbitrary starting point; usually measured in radians or degrees. In network analysis, the phase response of the device under test is the change in phase as a function of frequency between the input stimulus and the measured response."
Port_extension="Redefining the reference plane to other than that established at calibration. A new reference plane is defined in seconds of delay from the test set port."
Port="The physical input or output connection of an instrument or device."
Postscript_files=".ps files Store bitmapped graphics in an encapsulated format for direct use by postscript printers."
Power_max_input="The upper limit to input power for which the specifications apply. Some specifications may have different levels of maximum inputs. For example, compression power maximum is usually higher than the harmonic distortion maximum."
Power_safe_input="The input power, usually in dBm, allowed without damaging the instrument."
Preset="A pre-defined instrument state (that also runs an analyzer self-test). The action of pushing the preset key."
Protocol="A set of conventions that specify how information will be formatted and transmitted on a network, and how machines on a network will communicate."
Q_factor="The ratio of energy stored to energy lost in a resonant circuit. High Q indicates a sharp resonance response over frequency"
Query="Any analyzer programming command having the distinct function of returning a response. These commands may end with a question mark (?)."
R_channel="Reference channel"
rjx="Expression for complex impedance, where r represents the resistive portion and x represents the reactive portion."
RAM="Random Access Memory, or read-write memory: A storage area allowing access to any of its storage locations. Data can be written to or retrieved from RAM, but data storage is only temporary. When the power is removed, the information disappears. User-generated information appearing on a display is RAM data."
Raw="Complex data from the start of the measurement."
Raw_Memory="Complex data saved to the memory buffer as a result of Data-To-Memory operation."
Receiver="A circuit or system designed for the reception and/or measurement of signals in a specified frequency spectrum."
Reference_level="An instrument function that allows the user to set the amplitude value at the reference position. On network analyzers, the reference position is also selectable."
Reference_Plane="The electrical location at which a network analyzer assumes the system connectors and fixturing ends and the DUT begins. The reference plane is set by using calibration standards with known electrical length. The closer the reference plane is to the device under test (DUT), the better the characterization of the device."
Reflection_coefficient="The ratio of the reflected voltage to the incident voltage into a transmission line or circuit. If a transmission line is terminated in its characteristic impedance, the reflection coefficient is zero. If the line is shorted or open, the coefficient is 1."
Reflection_measurements="Measurements that characterize the input and /or output behavior of the device under test (DUT). Measured as the ratio of the reflected signal to the incident signal as a function of frequency. Parameters are called return loss, reflection coefficient, impedance, and standing wave ratio (SWR), all as a function of frequency."
Reflection="The phenomenon in which a traveling wave strikes a discontinuity and returns to the original medium."
Remote_operation="A mode of operation where a computer controls an instrument. Front panel operation is called local operation"
Resolution="The ability of a receiver to resolve two signals."
Return_loss="The amount of dB that the reflected signal is below the incident signal. If zero signal is reflected, the impedance of the device is equal to the characteristic impedance of the transmission system, and return loss is infinite. If the entire incident signal is reflected, the return loss is zero."
Reverse_measurement="The measurement of a device from output to input."
RF="Radio Frequency (from approximately 50 kHz to approximately 3 GHz). Usually referred to whenever a signal is radiated through the air."
ROM="Read Only Memory: A storage area that can be read only; it cannot be written to or altered by the user. In instruments, the storage area that contains the 'brains' or operational programming; the firmware."
Sampler_Bounce="The leakage or crosstalk between a network analyzer's samplers. Delay in this crosstalk caused by leakage transmission propagation, give the interference its 'bounce' appearance. Sampler bounce causes an increase in the noise level of the affected channel, reducing the sensitivity of the analyzer."
Sampler="An electronic component that captures the signal level and phase across a known impedance at a uniform rate. In Network Analyzers, this sampling rate must be sufficiently high and precisely timed to make accurate measurements. Network analyzers typically have three or four samplers."
Scalar="A quantity that has magnitude but no phase. A network analyzer capable of measuring only magnitude."
SCPI="Standard Commands for Programmable Instruments."
Screen="The physical surface of the CRT or flat panel upon which the measurement results, setup information, and other instrument communication is presented."
Self_Test="A group of tests performed at power-up (or at preset) that verity proper instrument operation."
Sensitivity="The minimum input signal required to produce a specified output signal having a specified signal-to-noise ratio, or other specified criteria."
Serial_prefix="The five-character prefix that begins an instrument serial number; used to represent versions of firmware or hardware changes that have occurred."
Server="A device that is configured to provide a service to other devices on a network, such as shared access to a file system or printer."
SNR="Signal-to-noise_ratio, SNR or S/N: The ratio of the amplitude of the desired signal to the amplitude of noise signals, usually expressed in dB and in terms of peak values for impulse noise and root-mean-square values for random noise."
Single_sweep="The analyzer sweeps once when trigger conditions are met. Each sweep is initiated by pressing an appropriate front panel key, or by sending a programming command. HOLD??"
Small_signal_gain_compression="A situation when the input signal's measured amplitude is less than its actual level due to overloading of the network analyzer's input mixer; the analyzer is operating nonlinearly. For broadband analyzer detectors, a signal other than the one under test can put the analyzer into this gain compressed mode, thereby making even lower level signals appear at a lower level than actual. The broadband mode measures all the power incident to the analyzer, not just the signals at the frequency of interest."
Smith_chart="A graphical mapping of the complex reflection coefficient into normalized complex impedance. Circles on the chart represent constant resistance and radiating lines orthogonal to the circles represent constant reactance. The center of the chart represents the characteristic impedance of the transmission system. Any point on the chart defines a single complex impedance. A line on the chart represents changing impedance over frequency."
SOLT="A calibration using four known standards: Short-Open-Load-Through. Also known as a full two-port calibration and 12-term error correction"
Source_amplitude_accuracy="The amplitude uncertainty, in dB, of the source power readout."
Source_amplitude_flatness="The amplitude flatness, in dB, of the source power over the frequency range specified."
Source_frequency_resolution="The smallest unit of frequency which can be set and/or measured, in Hz."
Source_frequency_time_base_accuracy="A measure of the analyzer's frequency stability measured in parts per million (ppm. or 1 part in 10E6). For example, a stability of ±5.0 ppm means that an analyzer will measure 1 MHz to an accuracy of ±5 x 10-6 x 10E6 Hz = +5 Hz."
Source_frequency_time_base_stability="A measure of the analyzer's time base accuracy over time and temperature. Typically the time base accuracy will be specified for 1 year. A typical temperature frequency stability is ±10 ppm for 250 C± 50 C."
Source_harmonics="The level of harmonics generated by the analyzer's signal source, in dBc from the fundamental."
Source_match="A measure of how close the signal source impedance is to the ideal transmission line impedance of the test system. Match is usually measured as return loss or standing wave ratio (SWR) of the source."
Source="A device that supplies signal power; a sweep oscillator or synthesized sweeper."
Span_accuracy="The uncertainty of the indicated frequency separation of any two signals on the display."
Span="The stop frequency minus the start frequency. The span setting determines the horizontal-axis scale of the analyzer display."
S_parameters="Scattering parameters - A convention used to characterize the way a device modifies signal flow using a network analyzer. A two port device has four S-parameters: forward transmission (S21), reverse transmission (S12), forward reflection (S11), and reverse reflection (S22)"
Standards_data="This is the data acquired for each standard defined by the caltype. (For a ONE PORT CAL this would be the data acquired for the open, short and load)." 
Step="The increment of change that results when you press the front panel step keys, ~ and ~, or by program commands."
Stop_start_frequency="Terms used in association with the stop and start points of the frequency measurement range. Together they determine the span of the measurement range."
Storage_disk="n internal or external digital storage disk for saving test data, instrument settings, IBASIC programs, and other measurement parameters. Storage formats include MS-DOS (R) and HPs standard LIF with binary, PCX, I-IP-GL, or ASCII data formats."
Storage_states="The number of settings, programs, traces, and other parameters available to be saved, cataloged, and recalled at any one time."
Structural_return_loss="Poor return loss in a cable due to a periodic fault such as a periodic dent caused by dropping the cable spool or by the cable pulling process during manufacture."
Supplemental_characteristics="Typical but non-warranted performance parameters, denoted as 'typical', 'nominal' or 'approximate'."
Sweep_mode="The way in which a sweep is initiated or selected: single, continuous, alternate, or chopped."
Sweep_type="The method of sweeping the source: linear, log, or frequency step."
Sweep="The ability of the source to provide a specified signal level over a specified frequency range in a specified time period. In data processing mode, a series of consecutive data point measurements, taken over a sequence of stimulus values."
Sweeper="A signal source that outputs a signal that varies continuously in frequency."
SWR="Standing wave ratio, calculated as (1 + p) / (1 -p) where p is the reflection coefficient."
Syntax="The grammar rules that specify how commands must be structured for an operating system, programming language, or applications."
System_dynamic_range="The difference between the maximum receiver input level and the receiver's noise floor. System dynamic range applies to transmission measurements only, since reflection measurements are limited by directivity."
T_R="Transmission/reflection (T/R): Refers to the suite of measurements made by a scalar or vector network analyzer to characterize a device's behavior over frequency."
Termination="A load connected to a transmission line or other device."
Test_limit="The acceptable result levels for any given measurement."
Test_set="The arrangement of hardware (switches, couplers, connectors and cables) that connect a test device input and output to the network analyzer's source and receiver to make s-parameter measurements."
Third_order_intercept="TOI: The power input to a non-linear device that would cause third order distortion at the same power level. TOI is a measurement to determine the distortion characteristics of a mixer or receiver. The higher the value, the more immune the receiver to internal distortion."
Thru="Through line: A calibration standard."
Toggle="To switch states, usually to change a function from on to off, or off to on."
Trace_noise="The amount of amplitude or phase uncertainty of the trace due to random noise in the test system."
Trace="A series of data points containing frequency and amplitude information."
Tracking="The ability of the analyzer's receiver to tune to the source frequency over the measurement frequency range. Poor tracking results in amplitude and phase errors due to the receiver IF circuits attenuating and delaying the device under test output."
Transfer_function="The ratio of the output signal to the input signal, both as a function of frequency."
Transmission_intermodulation_spurious="A measure of the capability of the transmitter to inhibit the generation of intermodulation distortion products. Intermodulation spurious is sometimes called intermodulation attenuation."
Transmission_measurements="The characterization of the transfer function of a device, that is, the ratio of the output signal to the incident signal. Most common measurements include gain, insertion loss, transmission coefficient, insertion phase, and group delay, all measured over frequency."
Transparent="Something that is not visible to the user. Usually a procedure that occurs without the user's initiation or knowledge."
Trigger="A signal that causes the instrument to make a measurement. The user can select several options for triggering, such as manual, continuous, or external (for synchronizing measurements to an external source)."
TRL_LRM="A calibration used in environments where the DUT cannot be connected directly to the network analyzer ports, (MMIC, microstrip, beam-lead diodes etc.). Thru-Reflect-Line (TRL) and Match(M) standards are fabricated and used because known high-quality standards are not readily available. The requirements for characterizing these standards are less stringent and can be more accurate than the traditional full two-port calibration using S-O-L-T standards. The terms are used interchangeably (TRL, LRL, LRM etc.) but they all refer to the same basic calibration method."
TRL="Through-Reflect-Line."
Note="CALCulate commands act on the selected measurement. You can select one measurement in each channel. Select the measurement for each channel using CALC:PAR:SEL."
Overlapped="Overlapped commands generally take extended time to process by the analyzer and MAY require 'synchronizing' the analyzer and controller. For more information, see 'Understanding Command Synchronization' (look under See Also...)"
DCOM="Distributed COM (DCOM)is a programming method where an application on one computer accesses an application running on another computer using a COM interface."
DefSCPI="The setting that occurs when the analyzer is Preset"
GPIBExamples="The GPIB examples use the National Instruments ComponentWorks(TM) 'Instr' ActiveX Control, Version 2.0.1 which is NO longer available. You must package the SCPI command in the appropriate syntax for your programming language."
