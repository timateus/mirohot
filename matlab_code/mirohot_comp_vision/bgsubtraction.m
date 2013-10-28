%This is an image processing algorithm for the MIROHOT project. 
%Algotith detects robots 
%For the setup a webcamera is needed, the field should be eliminated with
%white light and reflective shapes should be placed at the top of each
%robot.
%
%Image Acquizition and Image Processing Toolboxes are required.
%
%
%
%
%
%
%Timofey Nosov
%Tnosov@sfu.ca
%September - October 2013


%% set up video aquisition
if ~exist('vid', 'var')  %check if the videoinput has been created
    vid = videoinput('winvideo', 1); %creates a videoinput, a webcam must be connected
    vid.ReturnedColorspace = 'grayscale'; %get a greyscale image
    
    triggerconfig(vid, 'manual');  
    start(vid)
    % preview(vid);
end
resolution = vid.VideoREsolution;

%% Constants

bwthreshold = 0.95;
area = 300; % minimal area of an object in px
t = 1000; % length of the experiment
looptime = zeros(t, 1);
path = zeros(t, 2);
path2 = zeros(t, 2);

%moment invariants (Hu moments) for each shape we will detect. Calculated in a separate script
avg_carrots = [0.2074    0.0105    0.0035    0.0005    0.0000    0.0000   -0.0000];
avg_circle = [0.4106    0.0002    0.0000    0.0001   -0.0000    0.0000    0.0000];
shapes_moments = [avg_carrots; avg_circle]; %contains moments for each shape

% bg = getsnapshot(vid);
% subplot(2,2,1)
% % figure(2)
% imshow(bg)
% axis([0 640 0 480])
% % hold on
% bg = im2bw(bg, bwthreshold);


%%
for i = 1:t
    %% Acquire image, do basic pre-processing
    tic
    snap = getsnapshot(vid);  
    snaporiginal = snap;
    snap = im2bw(snap, bwthreshold); % convert to binary with specified threshold
    
    %create a gaussian filter and smooth an image 
    gaussFilter = fspecial('gaussian',[3 3], 0.5);
    snap = imfilter(snap, gaussFilter);
    
%     %plot bw picture
%     subplot(2,2,2)
%     imshow(snap)  
%     hold on  
    
    %% More advanced image processing
    % find connected components and analyze them
    CC = bwconncomp(snap); 
    STATS = regionprops(CC, 'centroid', 'Area', 'BoundingBox', 'Orientation'); 

    %find and get rid of conected components smaller than paremeter area
    areas = cat(1, STATS.Area);
    ind = find(areas < area); %indeces of connected components less than desired area
    CC.PixelIdxList(ind) = []; %delete conected components with indeces with area less then specified
    CC.NumObjects = CC.NumObjects - length(ind);
    STATS(ind,:) = []; %delete properties of components with too small area
    
    % rearrange data
    centroids = cat(1, STATS.Centroid);
    areas = cat(1, STATS.Area);
    boxes = round(cat(1, STATS.BoundingBox));

    areas;
    centroids;
    boxes;
    
    %% Calculate Hu moments of each blob
    if CC.NumObjects == 1 %if we have one object
        img = zeros(CC.ImageSize); %create an empty matrix
        img(CC.PixelIdxList{1,1}) = 1; % fill it with one blob
        humoment1 = humoments(img); % calculate moments of the image
        current_moments = [humoment1; shapes_moments];
        distance = pdist(current_moments, 'correlation'); %distance between shapes vectors and current vector 
        if distance(1, 1) < distance (1,2) 
            fprintf('I see a carrot!\n')
        else 
            fprintf('I see a circle!\n')
        end       
    end
    
    if CC.NumObjects == 2 %if we have two objects     
        img = zeros(CC.ImageSize); %create an empty matrix
        img(CC.PixelIdxList{1,2}) = 1; % fill it with one blob
        humoment1 = humoments(img); % calculate moments of the image
        current_moments = [humoment1; shapes_moments];
        distance = pdist(current_moments, 'correlation'); %distance between shapes vectors and current vector 
        if distance(1, 1) < distance (1,2) 
            fprintf('...and a carrot!\n')
        else 
            fprintf('...and a circle!\n')
        end   
    end
    
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
    
    %% Moments of the components
    %Calculate moments of the picture. Humoments function is in a separate
    %file
    current_moments = humoments(snap);
    current_moments = [current_moments; shapes_moments];
    
    %%
    %a guestimate for mini-wec competition. Completely irrelevant
    (17 * 5.5) * 0.4  - (3 + 1) * 4 - 12;

    %%
    looptime(i, 1) = toc; %time for each loop itteration

end


%% visualization
figure
plot(looptime)
title('looptime')
hold on
averagetime = mean(looptime)

figure 
plot(path(:,1), -path(:,2))
title('path1')
axis([0 640 -480 0])

figure 
plot(path2(:,1), -path2(:,2))
title('path2')
axis([0 640 -480 0])

% this function for mapping 
% fitgeotrans

% this to display arrow
% http://stackoverflow.com/questions/1803043/how-do-i-display-an-arrow-positioned-at-a-specific-angle-in-matlab
