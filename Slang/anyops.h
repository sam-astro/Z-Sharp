
#ifndef ANYOPS_H
#define ANYOPS_H

using namespace boost;

// Will convert type 'any' val to a bool
bool AnyAsBool(any val)
{
	try // Try converting to bool
	{
		return any_cast<bool>(val);
	}
	catch (const std::exception&)
	{
		try // Try converting to string
		{
			return any_cast<string>(val) == "true";
		}
		catch (const std::exception&)
		{
			try // Try converting to float
			{
				return any_cast<float>(val) == 1.0f;
			}
			catch (const std::exception&) // Try converting to int
			{
				try
				{
					return any_cast<int>(val) == 1;
				}
				catch (const std::exception&) // Does not convert, return
				{
					return false;
				}
			}
		}
	}
	return false;
}

// Will convert type 'any' val to a string
string AnyAsString(any val)
{
	try // Try converting to string
	{
		return any_cast<string>(val);
	}
	catch (const std::exception&)
	{
		try // Try converting to int
		{
			return to_string(any_cast<int>(val));
		}
		catch (const std::exception&)
		{
			try // Try converting to float
			{
				return to_string(any_cast<float>(val));
			}
			catch (const std::exception&) // Try converting to bool
			{
				try
				{
					string i = "false";
					if (any_cast<bool>(val) == true) i = "true";
					return i;
				}
				catch (const std::exception&) // Does not convert, return
				{
					return "";
				}
			}
		}
	}
	return "";
}

// Will convert type 'any' val to a float
float AnyAsFloat(any val)
{
	try // Try converting to float
	{
		return any_cast<float>(val);
	}
	catch (const std::exception&)
	{
		try // Try converting to int
		{
			return (float)any_cast<int>(val);
		}
		catch (const std::exception&)
		{
			try // Try converting to string, then converting it to float
			{
				return stof(any_cast<string>(val));
			}
			catch (const std::exception&) // Try converting to bool
			{
				try
				{
					float i = 0;
					if (any_cast<bool>(val) == true) i = 1;
					return i;
				}
				catch (const std::exception&) // Does not convert, return
				{
					return 0;
				}
			}
		}
	}
	return 0;
}

// Will convert type 'any' val to an integer
int AnyAsInt(any val)
{
	try // Try converting to int
	{
		return any_cast<int>(val);
	}
	catch (const std::exception&)
	{
		try // Try converting to float
		{
			return (int)any_cast<float>(val);
		}
		catch (const std::exception&)
		{
			try // Try converting to string, then converting it to int
			{
				return stoi(any_cast<string>(val));
			}
			catch (const std::exception&) // Try converting to bool
			{
				try
				{
					int i = 0;
					if (any_cast<bool>(val) == true) i = 1;
					return i;
				}
				catch (const std::exception&) // Does not convert, return
				{
					return 0;
				}
			}
		}
	}
	return 0;
}

// Gets type of 'any' val
// 0 -> int;  1 -> float;  2 -> bool;  3 -> string;
int any_type(any val)
{
	try // Try converting to int
	{
		int i = any_cast<int>(val);
		return 0;
	}
	catch (const std::exception&)
	{
		try // Try converting to float
		{
			float f = any_cast<float>(val);
			return 1;
		}
		catch (const std::exception&)
		{
			try // Try converting to bool
			{
				bool b = any_cast<bool>(val);
				return 2;
			}
			catch (const std::exception&) // Try converting to string
			{
				try
				{
					string s = any_cast<string>(val);
					return 3;
				}
				catch (const std::exception&) // Does not convert, return
				{
					return -1;
				}
			}
		}
	}
	return -1;
}

#endif