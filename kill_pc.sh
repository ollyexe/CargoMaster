start_pid=1
end_pid=100000

# Loop through the range and send a signal (e.g., SIGTERM) to each PID
for ((pid=start_pid; pid<=end_pid; pid++)); do
    kill -TERM $pid
done