function [CC, STATS] = getCC(vid, bwthreshold)


    snap = getsnapshot(vid);  
    snaporiginal = snap;
    snap = im2bw(snap, bwthreshold); % convert to binary with specified threshold
    
%     %create a gaussian filter and smooth an image 
%     gaussFilter = fspecial('gaussian',[3 3], 0.5);
%     snap = imfilter(snap, gaussFilter);
    
%     %plot bw picture
%     subplot(2,2,2)
    figure(1)
    imshow(snap)  
%     hold on  
    
    %% More advanced image processing
    % find connected components and analyze them
    CC = bwconncomp(snap); 
    STATS = regionprops(CC, 'centroid', 'Area', 'Orientation');
    CC.NumObjects
    
end
