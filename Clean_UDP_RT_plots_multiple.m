%%------------------------------------------------- Create the UDP object
%------------------------- 2020b  
u = udpport("datagram", "IPV4", "LocalHost", "10.183.93.17", "LocalPort", 61111);
%%---------------------------------------------- Set up the figure window
%time = now;
T = 0;
RH = 0;
CO2eq = 0;
TVOC = 0;

% %--------------------------------------------- Set up tile window for multiple figures
t = tiledlayout(4,1);               % >2019b
                                    % (rows, columns)
                                                                     
ax1 = nexttile;                     %creates an axes object
                                    %places it into the next empty tile
                                    %in the current figure                                                                          
title('Temperature')
ylabel('C')
hold on                             %preserved after call of plot()

ax2 = nexttile;
title('RH')
ylabel('%')
hold on

ax3 = nexttile;
title('CO2eq')
ylabel('ppm')
hold on

ax4 = nexttile;
title('TVOC')
ylabel('ppb')
hold on

%%------------------------- Set time span and interval for data collection
stopTime = '12/01 13:40';           %effectively non-stop
%%----------------------------------------------------------- Collect data
count = 1;

loops = 0;

while ~isequal(datestr(now,'mm/DD HH:MM'),stopTime)
        
    time(count) = datetime('now');
    %------------------------------------- 2020b - read received packet 
    readValue = read(u, 1, "string");             %number of datagram = 1

    %------------------------------------------------- extract strings
    str_all = extractBefore(readValue.Data, "O");
    str_T = extractBefore(str_all, "T");
    str_RH = extractBetween(str_all, "T", "R");
    str_CO2eq = extractBetween(str_all, "H", "C");
    str_TVOC = extractBetween(str_all, "C", "V");
    
    %-------------------------------------------------- convert to numbers
    T(count) = str2double(str_T);
    RH(count) = str2double(str_RH);
    CO2eq(count) = str2double(str_CO2eq);
    TVOC(count) = str2double(str_TVOC);   
    
    %---------------------------------------------------------update plots
    plot(ax1, time,T, '-r');              %auto-target current figure
    plot(ax2, time,RH, '-k');             %auto-target current figure
    plot(ax3, time,CO2eq, '--b');          %auto-target current figure
    plot(ax4, time,TVOC, '-m');           %auto-target current figure
    
    pause(0.1);
    count = count +1;
    
    %--------------------------------------------- Specify no. of readings
%     loops = loops + 1;
%     
%     if loops >= 10          %loops = no. of readings
%         %-------------------------------------- write to file
%         maTrix = [T;RH;CO2eq;TVOC];
%         writematrix(maTrix,'data_1.csv')
%         break
%     end
end
%% Clean up the UDP object
delete(u)