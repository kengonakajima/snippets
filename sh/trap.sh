sleep 10 &


trap 'kill $(jobs -p)' EXIT

jobs
