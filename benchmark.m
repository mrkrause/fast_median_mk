%% Benchmark various built-in methods of finding the median vs. fast_median
SAMPLE_SIZE = floor(linspace(10, 10^7, 50));
N_REPS = 50 ;

t_builtin = nan(length(SAMPLE_SIZE), N_REPS);
t_fast    = nan(length(SAMPLE_SIZE), N_REPS);
t_sort    = nan(length(SAMPLE_SIZE), N_REPS);
t_prctile = nan(length(SAMPLE_SIZE), N_REPS);

% Warm-up: force matlab to find/load the functions
median(1:3);
fast_median(1:3);
sort(1:3);

for n = 1:N_REPS
    for s =1:length(SAMPLE_SIZE)
        data = rand(SAMPLE_SIZE(s), 1);
        
        tic();      
        m=fast_median(data);     
        t_fast(s,n) = toc();
        
        tic();        
        m=median(data);            
        t_builtin(s,n) = toc();    
        
        tic();
        sorted=sort(data);        
        t_sort(s,n) = toc();
    end
    fprintf('Rep %d complete\n', n);
end


%% For benchmarking,  it sometimes makes more sense to look at the minimum
% time, rather than the average. Many factors can make code run slower than
% expected (e.g., background processes), but not much can  "accidentally" 
% accelerate it so it finishes early. Still, pick your poison below:

reduce = @(x) min(x, [], 2);
% reduce = @(x) nanmean(x, 2);

plot(SAMPLE_SIZE, reduce(t_fast), SAMPLE_SIZE, reduce(t_builtin), SAMPLE_SIZE, reduce(t_sort));
xlabel('Number of data points');
ylabel(sprintf('Running time (sec, min of %d repeats)', N_REPS));
verinfo = ver('MATLAB');
title(sprintf('Running on MATLAB %s %s-%s', verinfo.Version, verinfo.Release, computer('arch')));
legend({'fast_median', 'built-in', 'sort'}, 'Interpreter', 'none');