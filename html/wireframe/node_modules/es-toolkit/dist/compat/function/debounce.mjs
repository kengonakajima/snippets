function debounce(func, wait = 0, options = {}) {
    if (typeof options !== 'object') {
        options = {};
    }
    let pendingArgs = null;
    let pendingThis = null;
    let lastCallTime = null;
    let debounceStartTime = 0;
    let timeoutId = null;
    let lastResult;
    const { leading = false, trailing = true, maxWait } = options;
    const hasMaxWait = 'maxWait' in options;
    const maxWaitMs = hasMaxWait ? Math.max(Number(maxWait) || 0, wait) : 0;
    const invoke = (time) => {
        if (pendingArgs !== null) {
            lastResult = func.apply(pendingThis, pendingArgs);
        }
        pendingArgs = pendingThis = null;
        debounceStartTime = time;
        return lastResult;
    };
    const handleLeading = (time) => {
        debounceStartTime = time;
        timeoutId = setTimeout(handleTimeout, wait);
        if (leading && pendingArgs !== null) {
            return invoke(time);
        }
        return lastResult;
    };
    const handleTrailing = (time) => {
        timeoutId = null;
        if (trailing && pendingArgs !== null) {
            return invoke(time);
        }
        return lastResult;
    };
    const checkCanInvoke = (time) => {
        if (lastCallTime === null) {
            return true;
        }
        const timeSinceLastCall = time - lastCallTime;
        const hasDebounceDelayPassed = timeSinceLastCall >= wait || timeSinceLastCall < 0;
        const hasMaxWaitPassed = hasMaxWait && time - debounceStartTime >= maxWaitMs;
        return hasDebounceDelayPassed || hasMaxWaitPassed;
    };
    const calculateRemainingWait = (time) => {
        const timeSinceLastCall = lastCallTime === null ? 0 : time - lastCallTime;
        const remainingDebounceTime = wait - timeSinceLastCall;
        const remainingMaxWaitTime = maxWaitMs - (time - debounceStartTime);
        return hasMaxWait ? Math.min(remainingDebounceTime, remainingMaxWaitTime) : remainingDebounceTime;
    };
    const handleTimeout = () => {
        const currentTime = Date.now();
        if (checkCanInvoke(currentTime)) {
            return handleTrailing(currentTime);
        }
        timeoutId = setTimeout(handleTimeout, calculateRemainingWait(currentTime));
    };
    const debouncedFunction = function (...args) {
        const currentTime = Date.now();
        const canInvoke = checkCanInvoke(currentTime);
        pendingArgs = args;
        pendingThis = this;
        lastCallTime = currentTime;
        if (canInvoke) {
            if (timeoutId === null) {
                return handleLeading(currentTime);
            }
            if (hasMaxWait) {
                clearTimeout(timeoutId);
                timeoutId = setTimeout(handleTimeout, wait);
                return invoke(currentTime);
            }
        }
        if (timeoutId === null) {
            timeoutId = setTimeout(handleTimeout, wait);
        }
        return lastResult;
    };
    debouncedFunction.cancel = () => {
        if (timeoutId !== null) {
            clearTimeout(timeoutId);
        }
        debounceStartTime = 0;
        lastCallTime = pendingArgs = pendingThis = timeoutId = null;
    };
    debouncedFunction.flush = () => {
        return timeoutId === null ? lastResult : handleTrailing(Date.now());
    };
    return debouncedFunction;
}

export { debounce };
