bytesCount str = case str of
	[] -> 0
	(c:cs) -> 1 + bytesCount cs
