%test file for figuring out how to map image input to desired coordinate
%system
bwthreshold = 0.95;
snap = imread('snap2');  
snaporiginal = snap;
snap = im2bw(snap, bwthreshold); % convert to binary with specified threshold


%% More advanced image processing
% find connected components and analyze them
CC = bwconncomp(snap); 
STATS = regionprops(CC, 'centroid', 'Area'); 

%find and get rid of conected components smaller than paremeter area
areas = cat(1, STATS.Area);
ind = find(areas < 50); %indeces of connected components less than desired area
CC.PixelIdxList(ind) = []; %delete conected components with indeces with area less then specified
CC.NumObjects = CC.NumObjects - length(ind);
STATS(ind,:) = []; %delete properties of components with too small area

% rearrange data
centroids = cat(1, STATS.Centroid);
areas = cat(1, STATS.Area);
areas;
centroids;
%%
%calculate the data to crop
topleftcorner = min(centroids);
bottomright = max(centroids);
cropsize = bottomright - topleftcorner;
% figure(3)
cropped = imcrop(snap, [topleftcorner, cropsize]); %  this is cropped image
imsize = size(cropped);
% imshow(cropped)
% title('cropped')

%new centroids
newcentroids(:,1) = centroids(:,1) - topleftcorner(1,1);
newcentroids(:,2) = centroids(:,2) - topleftcorner(1,2);
%sort centroids in the order: top left, botoom left, top right, bottom
%right
sumxy = sum(newcentroids,2); %sum of x and y coordinates of each point
[temp,indeces] = sort(sumxy);
sortedcentroids = newcentroids(indeces,:);


%%
%calculate transform
distortedpts = [sortedcentroids];
original = [1 1; 1 imsize(1); imsize(2) 1; imsize(2) imsize(1)];
tform  = estimateGeometricTransform(distortedpts, original, 'similarity');
tform2 = cp2tform(distortedpts, original, 'projective');


B = imwarp(cropped,tform);
[xm,ym] = tformfwd(tform2, [distortedpts(:,1); 100], [distortedpts(:,2); 100]);



%% output
figure(1)
subplot(2,2,1)
imshow(cropped)
title('original');
axis image
subplot(2,2,2)
imshow(B)
title('restored');
axis image


subplot(2,2,3)
plot(distortedpts(:,1), distortedpts(:,2), 'or')
title('original');
axis image
subplot(2,2,4)
plot(xm,ym, 'or')
title('restored');
axis image




% visiongeotforms