function [tform2] = perspectivecorrection(vid, areathreshold, bwthreshold)

% illustration of how to map image input to desired coordinate system
%
%
%
% Timofey Nosov
% October 29th, 2013

addpath('C:\Users\tnosov\Dropbox\CO-OP2013\not shared\Code\matlab_code\mirohot_comp_vision')

actualsize = [580, 1180]; % mm

tic
% bwthreshold = 0.95;
% area = 50;
snap = imread('snap2');  
snaporiginal = snap;
snap = im2bw(snap, bwthreshold); % convert to binary with specified threshold


[CC, STATS] = getCC(vid, bwthreshold);

[CC, STATS, areas, centroids] = processCC(CC, STATS, areathreshold);


%% crop img 
topleftcorner = min(centroids);
bottomright = max(centroids);
cropsize = bottomright - topleftcorner;
% figure(3)
cropped = imcrop(snap, [topleftcorner, cropsize]); %  this is cropped image
imsize = size(cropped);
% imshow(cropped)
% title('cropped')

%% recalculate centroids
newcentroids(:,1) = centroids(:,1) - topleftcorner(1,1);
newcentroids(:,2) = centroids(:,2) - topleftcorner(1,2);
%sort centroids in the order: top left, botoom left, top right, bottom
%right
sumxy = sum(newcentroids,2); %sum of x and y coordinates of each point
[temp,indeces] = sort(sumxy);
sortedcentroids = newcentroids(indeces,:);


%% calculate transform
distortedpts = [sortedcentroids];
original = [1 1; 1 actualsize(1); actualsize(2) 1; actualsize(2) actualsize(1)]; % here should be coordinates of ideal non-distorted table
% tform  = estimateGeometricTransform(distortedpts, original, 'similarity');
tform2 = cp2tform(distortedpts, original, 'projective');


% B = imwarp(cropped,tform);
[xm,ym] = tformfwd(tform2, [distortedpts(:,1)], [distortedpts(:,2)]);

% for k = 1:CC.NumObjects

% [row, col] = ind2sub(CC.ImageSize, PixelIdxList(1));

% end

    


toc
%% output
figure(1)
subplot(2,2,1)
imshow(snaporiginal)
title('original snapshot')

subplot(2,2,2)
imshow(snap)
title('corner detection')

subplot(2,2,3)
plot(distortedpts(:,1), distortedpts(:,2), 'or')
title('original');
axis image
subplot(2,2,4)
plot(xm,ym, 'or')
title('corrected');
axis image

end
