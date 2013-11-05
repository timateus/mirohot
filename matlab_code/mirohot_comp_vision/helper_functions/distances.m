function [distance, C, I] = distances(PixelIdxList, imgsize, shapes_moments)

        img = zeros(imgsize); %create an empty matrix
        img(PixelIdxList) = 1; % fill it with one blob
        humoment1 = humoments(img); % calculate moments of the image
        current_moments = [humoment1; shapes_moments];
        distance = pdist(current_moments, 'euclidean'); %distance between shapes vectors and current vector 
        distance(size(shapes_moments,1) + 1:end) = [];
        [C,I] = min(distance); % find smallest distance
end
