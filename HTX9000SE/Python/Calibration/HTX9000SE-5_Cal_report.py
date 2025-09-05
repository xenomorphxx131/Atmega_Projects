import datetime, time
from PyICe import lab_core, lab_instruments, lab_utils, LTC_plot
print("Connect load box to low voltage lab_instruments supply.")
HTX9000_comport_number = input("Enter HTX9000 COM Port Number: COM")
A34401_comport_number = input("Enter A34401 COM Port Number: COM")
master = lab_core.master()

a34401a = lab_instruments.agilent_34401a(master.get_visa_serial_interface("COM"+A34401_comport_number, baudrate = 9600, dsrdtr = True, timeout = 5))
a34401a.config_dc_current()
a34401a.add_channel("i_sense")

loadbox=lab_instruments.htx90000SE_5A(master.get_visa_serial_interface("COM" + HTX9000_comport_number, timeout=1))
loadbox.add_channel(channel_name="i_force", add_extended_channels=True)

master.add(a34401a)
master.add(loadbox)
serial_number=loadbox.get_serial_number()
now = datetime.datetime.now()

#############################################
# Low Range
#############################################
a34401a.config_dc_current(NPLC=10, range=0.1)
force_current_low=[]
sense_current_low=[]
for target_current in lab_utils.logRangeInc(start=400e-9,stop=100e-6,stepsPerDecade=25):
    master.write("i_force", target_current)
    time.sleep(1 + 20 / target_current * 400e-9) # is the load box super slow at low currents?
    meter = master.read("i_sense")
    setting = master.read("i_force_readback")
    sense_current_low.append(meter)
    force_current_low.append(setting)
    print(f"The target current is currently {target_current}", f"Load Box set to: {setting}", f"Meter Reading is: {meter}")
a34401a.config_dc_current(NPLC=10, range=0.1)
for target_current in lab_utils.logRangeInc(start=100e-6,stop=25e-3,stepsPerDecade=25):
    master.write("i_force", target_current)
    time.sleep(1)
    meter = master.read("i_sense")
    setting = master.read("i_force_readback")
    sense_current_low.append(meter)
    force_current_low.append(setting)
    print(f"The target current is currently {target_current}", f"Load Box set to: {setting}", f"Meter Reading is: {meter}")

#############################################
# High Range
#############################################
a34401a.config_dc_current(NPLC=10, range=3)
force_current_hi=[]
sense_current_hi=[]
for target_current in lab_utils.logRangeInc(start=25e-3,stop=3,stepsPerDecade=25):
    master.write("i_force", target_current)
    time.sleep(1)
    meter = master.read("i_sense")
    setting = master.read("i_force_readback")
    sense_current_hi.append(meter)
    force_current_hi.append(setting)
    print(f"The target current is currently {target_current}", f"Load Box set to: {setting}", f"Meter Reading is: {meter}")
            
master.write("i_force", 0)
            
error=[(i_sense/i_force-1)*100 for i_sense,i_force in zip(sense_current_hi,force_current_hi)]
result_hi=[x for x in zip(force_current_hi,error)]

error=[abs((i_sense/i_force-1)*100) for i_sense,i_force in zip(sense_current_low,force_current_low)]
result_low=[x for x in zip(force_current_low,error)]

G0 = LTC_plot.plot( plot_title  = f"Serial#:{serial_number}\nHigh Range Cal Report",
                    plot_name   = None,
                    xaxis_label = "CURRENT (A)",
                    yaxis_label = "ERROR (%)",
                    xlims       = (0.025, 3),
                    ylims       = (-.5, .5),
                    xminor      = 10,
                    xdivs       = 5,
                    yminor      = 0,
                    ydivs       = 8,
                    logx        = True,
                    logy        = False)
                                       
G1 = LTC_plot.plot( plot_title  = "Low Range Cal Report",
                    plot_name   = None,
                    xaxis_label = "CURRENT (A)",
                    yaxis_label = "ERROR (%)",
                    xlims       = (250e-9, 0.025),
                    ylims       = (0.001, 100),
                    xminor      = 10,
                    xdivs       = 5,
                    yminor      = 0,
                    ydivs       = 8,
                    logx        = True,
                    logy        = True)                   

smoothed_data_hi=lab_utils.ordered_pair(result_hi)
smoothed_data_hi.smooth_y(window=5, extrapolation_window=9, iterations=3)

smoothed_data_low=lab_utils.ordered_pair(result_low)
smoothed_data_low.smooth_y(window=5, extrapolation_window=5, iterations=3)

G0.add_trace(   axis        = 1,
                data        = result_hi,
                color       = LTC_plot.LT_RED_1,
                marker      = '.',
                markersize  = 1,
                legend      = "")

G0.add_trace(   axis        = 1,
                data        = smoothed_data_hi,
                color       = LTC_plot.LT_GREEN_1,
                marker      = '.',
                markersize  = 1,
                legend      = "")
                
G1.add_trace(   axis        = 1,
                data        = result_low,
                color       = LTC_plot.LT_BLUE_1,
                marker      = '.',
                markersize  = 1,
                legend      = "")
G1.add_trace(   axis        = 1,
                data        = smoothed_data_low,
                color       = LTC_plot.LT_YELLOW_2,
                marker      = '.',
                markersize  = 1,
                legend      = "")                


# G0.add_note(note=f"Serial#:{loadbox.get_serial_number()}", location = [0.05, 0.5], use_axes_scale = True)
G1.add_note(note=f"{now.strftime('%Y-%m-%d')}", location = [0.05, 0.5], use_axes_scale = True)

page = LTC_plot.Page(plot_count=2)
page.add_plot(G0)
page.add_plot(G1)
page.create_svg(file_basename=f"CAL_REPORT_{serial_number}")

print(f"\n\nGenerated report: CAL_REPORT_{serial_number}.svg")

 
