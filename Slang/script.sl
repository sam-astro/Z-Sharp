int x = 1

void Main(input)
{
	string c = 8
	string d = 8
	string e = 8
	string f = 8
	string g = 8
	string h = 8
	string i = 8
	string j = 8
	c /= 2
	d /= 2
	e /= 2
	f /= 2
	g /= 2
	h /= 2
	i /= 2
	g /= 2
	print input
	while x < 10000
	{
		g /= 2
		h /= 2
		i /= 2
		g /= 2
		x += 1
		if x > 0
		{
			print x
		}
	}
	x += 1
	print x
    x -= 1
	print x
}

Main "hi"