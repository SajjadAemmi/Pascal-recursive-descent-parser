program example(input, output);
var x, y: integer;
function gcd(a, b: integer): result integer;
begin 
	if b = 0 then gcd:= a 
	else gcd:= gcd(b, a mod b)
end
begin
	READ(x, y); 
	WRITE(gcd(x, y)) 
end
