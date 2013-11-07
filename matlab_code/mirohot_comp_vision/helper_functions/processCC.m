function [CC, STATS, areas, centroids] = processCC(CC, STATS, minarea, maxarea)
    


    
    % find and get rid of conected components smaller than paremeter area
    
    areas = cat(1, STATS.Area);
    ind = find(areas < minarea); % indeces of connected components less than desired area
    ind = [ind; find(areas > maxarea)];
    CC.PixelIdxList(ind) = []; % delete conected components with indeces with area less then specified
    CC.NumObjects = CC.NumObjects - length(ind); % update num of objects
    STATS(ind,:) = []; % delete properties of components with too small area
    
    % rearrange data
    centroids = cat(1, STATS.Centroid);
    areas = cat(1, STATS.Area);

    areas;
    centroids;
    
end