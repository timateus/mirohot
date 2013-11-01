bwthreshold = 0.95;
area = 40; % minimal area of an object in px
t = 400; % length of the experiment

%% set up video aquisition
fprintf('initializing video...\n')
if ~exist('vid', 'var')  %check if the videoinput has been created
    vid = videoinput('winvideo', 1); %creates a videoinput, a webcam must be connected
    vid.ReturnedColorspace = 'grayscale'; %get a greyscale image
    
    triggerconfig(vid, 'manual');  
    start(vid)
    % preview(vid);
end
resolution = vid.VideoREsolution;
fprintf('Initializing done.\n')

% preview(vid);

% either load precalculater moments or use this program to calculate them 
figure(2)
scatter3(carrotmoments(:,1), carrotmoments(:,2), carrotmoments(:,3), 'r')
hold on
scatter3(tapemoments(:,1), tapemoments(:,2), tapemoments(:,3), 'b')
scatter3(ballmoments(:,1), ballmoments(:,2), ballmoments(:,3), 'm')

% xlabel('first moment')
% ylabel('second moment')
% zlabel('third moment')
% title('first three moments of shapes')
% legend('carrot','tape','ball', 'Location','NorthEastOutside')
drawnow;
hold on;


for i = 1:t
    snap = getsnapshot(vid);
    snap = im2bw(snap, bwthreshold);
%     imshow(snap)
    CC = bwconncomp(snap); 
    STATS = regionprops(CC, 'centroid', 'Area');
    
    %find and get rid of conected components smaller than paremeter area
    areas = cat(1, STATS.Area);
    ind = find(areas < area); %indeces of connected components less than desired area
    CC.PixelIdxList(ind) = []; %delete conected components with indeces with area less then specified
    CC.NumObjects = CC.NumObjects - length(ind);
    STATS(ind,:) = []; %delete properties of components with too small area
    
    % rearrange data
    centroids = cat(1, STATS.Centroid);
    areas = cat(1, STATS.Area);

    areas;
    centroids;
    
    
    %% Calculate Hu moments of each blob
    
    if CC.NumObjects == 1
        
        img = zeros(CC.ImageSize); %create an empty matrix
        img(CC.PixelIdxList{1,1}) = 1; % fill it with one blob
        humoments1(i,:) = humoments(img); % calculate moments of the image
        figure(2)
        plot3(humoments1(i,1), humoments1(i,2), humoments1(i,3), 'k*')
        drawnow;
        hold on;
    end 
    
    
end
carrotmoments2 = humoments1;



% plot3(carrotmoments2(:,1), carrotmoments2(:,2), carrotmoments2(:,3), '*m')
% plot3(tapemoments(:,1), tapemoments(:,2), tapemoments(:,3), '*r')
% plot3(ballmoments(:,1), ballmoments(:,2), ballmoments(:,3), '*b')

