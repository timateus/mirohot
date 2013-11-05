%This is an image processing algorithm for the MIROHOT project. 
%Algotith detects robots 
%For the setup a webcamera is needed, the field should be illuminated with
%white light and reflective shapes should be placed at the top of each
%robot.
%
%Image Acquizition and Image Processing Toolboxes are required.
%
%Included files: processCC.m, getCC.m, distances.m
%
%
%
%
%Timofey Nosov
%Tnosov@sfu.ca
%September - October 2013



addpath('C:\Users\tnosov\Dropbox\CO-OP2013\not shared\Code\matlab_code\mirohot_comp_vision\helper_functions');
close all; % close all open windows
pause on; % enable to pause the program
%% set up video aquisition
fprintf('initializing video...\n')
if ~exist('vid', 'var')  %check if the videoinput has been created
    vid = videoinput('winvideo', 1); % create a videoinput, a webcam must be connected
    vid.ReturnedColorspace = 'grayscale'; % get a greyscale image
    
    triggerconfig(vid, 'manual');  
    start(vid)
    % preview(vid);
end
resolution = vid.VideoREsolution;
fprintf('Initializing done.\n')

%% Variable declaration and Constants

bwthreshold = 0.95;
area = 300; % minimal area of an object in px
t = 400; % length of the experiment
looptime = zeros(t, 1);
path = zeros(t, 2);
path2 = zeros(t, 2);
carrotdist = zeros(t, 1);
tapedist = zeros(t, 1);

% load('ballmoments.mat')
load('carrotmoments.mat')
load('tapemoments.mat')

% centreball = mean(ballmoments);
centrecarrot = mean(carrotmoments);
centretape = mean(tapemoments);

shapes_moments = [centrecarrot; centretape]; %contains moments for each shape

%% Body of the program
for i = 1:t    
    
    if i == 200 || i == 400 || i == 600
        pause
    end
    tic
    
    %% Basic Image Processing   
    % Acquire image, do basic pre-processing
    [CC, STATS] = getCC(vid, bwthreshold);
    % Rearrange data, find connected components with area more than "area"
    [CC, STATS, areas, centroids] = processCC(CC, STATS, area);    
    
%     if CC.NumObjects == 0
%         fprintf('no objects detected\n')
%     end
    
    for k = 1:CC.NumObjects
        
        % this function calculates distances from mean moments of the
        % objects to moments of current object
        [distance, C, I] = distances(CC.PixelIdxList{1,k}, CC.ImageSize, shapes_moments);  
      
        if C > 0.04 % if the smallest distance is bigger than a threshold, then most probably it's not one of the objects
            fprintf('cannot recognize the shape\n')
        else
            if I == 1
                fprintf('carrot\n')
            elseif I == 2
                fprintf('tape\n')
            end
        end
        carrotdist(i) = distance(1);
        tapedist(i) = distance(2);  
    end        
 
    i;
   
    %% calculating and real time ploting of the path
    if size(centroids,1)>0
        path(i,:) = centroids(1,:);
        figure(1)
        subplot(2,1,1)
        plot(path(i,1), -path(i,2), 'or')
        title('path1')
        axis([0 640 -480 0])
    end

    if size(centroids,1)>1
        path2(i,:) = centroids(2,:);
        subplot(2,1,2)
        plot(path2(i,1), -path2(i,2), 'or')
        title('path2')
        axis([0 640 -480 0])
    end   

    %% 
    looptime(i, 1) = toc; %time for each loop itteration
    looptime(i, 1);
end


%% visualization
figure(2)
plot(looptime)
title('looptime')
hold on
averagetime = mean(looptime);

% here two loop to delete all locations with coordinates [0,0] for better
% looking plotting
index = [];
for k = 1:size(path)
    if (path(k,2) == 0) && (path(k,1) == 0)
        index = [index; k];
    end
end
path(index, :) = [];
index = [];
for k = 1:size(path2)
    if (path2(k,2) == 0) && (path2(k,1) == 0)
        index = [index; k];
    end
end
path2(index, :) = [];

figure(3)
subplot(2,1,1)
plot(path(:,1), -path(:,2))
title('path1')
axis([0 640 -480 0])

subplot(2,1,2)
plot(path2(:,1), -path2(:,2))
title('path2')
axis([0 640 -480 0])


%Distances from mean moments of the objects to current object 
%predefined objects' moments
figure(4)
% plot( 1:i, (2.4022e-05) * 2, 'r-')
plot(carrotdist, 'r')
hold on
plot(tapedist, 'b')
% plot( 1:i, (4.6460e-05) * 2, 'm-')
title('some distances')
plot(1:t(end), 0.06, 'k')


