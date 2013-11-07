function [topleftcorner, tform2, cornercentroids] = perspectivecorrection(vid, minarea, maxarea, bwthreshold)
% tic
% finds transform to map image input to a coordinate system with origin at
% top left corner and mm as units
%
%
% Timofey Nosov
% October 29th, 2013


actualsize = [580, 1180]; % mm

[CC, STATS] = getCC(vid, bwthreshold);

[CC, STATS, areas, centroids] = processCC(CC, STATS, minarea, maxarea);

if length(centroids) ~= 4
    warning('Number of identified objects is not equal to 4. Restart the program with correct setup: four markers at corners and no other markers')
    pause
end

%% crop img 
cornercentroids = centroids;
topleftcorner = min(centroids);
% bottomright = max(centroids);
% cropsize = bottomright - topleftcorner;
% figure(3)
% cropped = imcrop(snap, [topleftcorner, cropsize]); %  this is cropped image
% imsize = size(cropped);
% imshow(cropped)
% title('cropped')

%% recalculate centroids
newcentroids(:,1) = centroids(:,1) - topleftcorner(1,1);
newcentroids(:,2) = centroids(:,2) - topleftcorner(1,2);
%sort centroids in the order: top left, botoom left, top right, bottom
%right
sumxy = sum(newcentroids,2); %sum of x and y coordinates of each point
[~,indeces] = sort(sumxy);
sortedcentroids = newcentroids(indeces,:);


%% calculate transform
distortedpts = sortedcentroids;
original = [1 1; 1 actualsize(1); actualsize(2) 1; actualsize(2) actualsize(1)]; % here are the dimentions of ideal non-distorted table


tform2 = cp2tform(distortedpts, original, 'projective');

[xm,ym] = tformfwd(tform2, distortedpts(:,1), distortedpts(:,2));



%% output
% figure
% subplot(2,2,3)
% plot(distortedpts(:,1), distortedpts(:,2), 'or')
% title('original');
% axis image
% subplot(2,2,4)
% plot(xm,ym, 'or')
% title('corrected');
% axis image
% toc


end
