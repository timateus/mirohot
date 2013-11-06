addpath('C:\Users\tnosov\Dropbox\CO-OP2013\not shared\Code\matlab_code\mirohot_comp_vision\hu_moments');

bwthreshold = 0.95;
area = 40; % minimal area of an object in px
t = 1000; % length of the experiment

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
%%

% either load precalculater moments or use this program to calculate them 
load('ballmoments.mat')
load('carrotmoments.mat')
load('tapemoments.mat')

centreball = mean(ballmoments);
centrecarrot = mean(carrotmoments);
centretape = mean(tapemoments);
centres = [centreball; centrecarrot; centretape];


% plot clouds of moments
figure(2)
scatter3(carrotmoments(:,1), carrotmoments(:,2), carrotmoments(:,3), 'r')
hold on
scatter3(tapemoments(:,1), tapemoments(:,2), tapemoments(:,3), 'b')
scatter3(ballmoments(:,1), ballmoments(:,2), ballmoments(:,3), 'm')

xlabel('first moment')
ylabel('second moment')
zlabel('third moment')
axis equal
title('first three moments of shapes')
legend('carrot','tape','ball', 'Location','NorthEastOutside')
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
    CC.NumObjects;
    
    %% Calculate Hu moments of each conected component
    
    if CC.NumObjects == 1
        
        img = zeros(CC.ImageSize); %create an empty matrix
        img(CC.PixelIdxList{1,1}) = 1; % fill it with one blob
        humoments1(i,:) = humoments(img); % calculate moments of the image
        figure(2)
        plot3(humoments1(i,1), humoments1(i,2), humoments1(i,3), 'k*')
        drawnow;
        hold on;
        
        centrematrix = [humoments1(i,:); centres]; % matrix containing hu moments of a new object and means for hu moments for known objects
        
        distances = pdist(centrematrix);
        distances(4:end) = []
        
        [C,I] = min(distances);
        
        if I == 1
            fprintf('this is a ball')
        elseif I == 2
            fprintf('this is a carrot')
        elseif I == 3
            fprintf('this is tape')
        end
        
            
                
            
                

        
        
    end 
    
    
end

% carrotmoments2 = humoments1;



% plot3(carrotmoments2(:,1), carrotmoments2(:,2), carrotmoments2(:,3), '*m')
% plot3(tapemoments(:,1), tapemoments(:,2), tapemoments(:,3), '*r')
% plot3(ballmoments(:,1), ballmoments(:,2), ballmoments(:,3), '*b')

