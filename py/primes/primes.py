from typing import List
from mcp.server.fastmcp import FastMCP

# Initialize FastMCP server
mcp = FastMCP("primes")


def is_prime(n: int) -> bool:
    """
    Check if a number is prime.
    
    Args:
        n: The number to check
        
    Returns:
        True if the number is prime, False otherwise
    """
    if n <= 1:
        return False
    if n <= 3:
        return True
    if n % 2 == 0 or n % 3 == 0:
        return False
    
    i = 5
    while i * i <= n:
        if n % i == 0 or n % (i + 2) == 0:
            return False
        i += 6
    
    return True


def sieve_of_eratosthenes(limit: int) -> List[int]:
    """
    Generate all prime numbers up to a given limit using the Sieve of Eratosthenes.
    
    Args:
        limit: Upper limit for prime number generation
        
    Returns:
        List of prime numbers up to the limit
    """
    sieve = [True] * (limit + 1)
    sieve[0] = sieve[1] = False
    
    for i in range(2, int(limit**0.5) + 1):
        if sieve[i]:
            for j in range(i*i, limit + 1, i):
                sieve[j] = False
    
    return [i for i in range(limit + 1) if sieve[i]]


@mcp.tool()
def count_primes(limit: int) -> int:
    """
    Count the number of prime numbers up to a given limit.
    
    Args:
        limit: Upper limit for prime number counting
    """
    if limit < 0:
        return "Error: Limit must be a non-negative integer."
    
    if limit > 10_000_000:
        return "Error: Limit is too large. Please use a value less than 10,000,000."
    
    primes = sieve_of_eratosthenes(limit)
    return len(primes)


@mcp.tool()
def list_primes(start: int, end: int) -> str:
    """
    List all prime numbers in a given range.
    
    Args:
        start: Lower bound of the range (inclusive)
        end: Upper bound of the range (inclusive)
    """
    if start < 0 or end < 0:
        return "Error: Range bounds must be non-negative integers."
    
    if end > 100_000:
        return "Error: Upper bound is too large. Please use a value less than 100,000."
    
    if start > end:
        return "Error: Start value must be less than or equal to end value."
    
    if end - start > 10_000:
        return "Error: Range is too large. Please specify a range of at most 10,000 numbers."
    
    primes = [n for n in range(max(2, start), end + 1) if is_prime(n)]
    
    if not primes:
        return "No prime numbers found in the specified range."
    
    return ", ".join(map(str, primes))


@mcp.tool()
def is_prime_number(number: int) -> str:
    """
    Check if a given number is prime.
    
    Args:
        number: The number to check
    """
    if number < 0:
        return "Error: Please provide a non-negative integer."
    
    if number > 10**12:
        return "Error: Number is too large. Please use a value less than 10^12."
    
    result = is_prime(number)
    
    if result:
        return f"{number} is a prime number."
    else:
        return f"{number} is not a prime number."


@mcp.tool()
def find_nth_prime(n: int) -> str:
    """
    Find the nth prime number.
    
    Args:
        n: The position of the prime number to find
    """
    if n <= 0:
        return "Error: Please provide a positive integer."
    
    if n > 1_000_000:
        return "Error: The requested position is too large. Please use a value less than 1,000,000."
    
    # For small values, we can use a predefined limit
    if n <= 10:
        primes = sieve_of_eratosthenes(30)
    elif n <= 100:
        primes = sieve_of_eratosthenes(550)
    elif n <= 1000:
        primes = sieve_of_eratosthenes(8000)
    elif n <= 10000:
        primes = sieve_of_eratosthenes(110000)
    elif n <= 100000:
        primes = sieve_of_eratosthenes(1300000)
    else:  # n <= 1_000_000
        primes = sieve_of_eratosthenes(20000000)
    
    if n <= len(primes):
        return f"The {n}th prime number is {primes[n-1]}."
    else:
        return "Error: Could not compute the requested prime number."


if __name__ == "__main__":
    # Initialize and run the server
    mcp.run(transport='stdio')
