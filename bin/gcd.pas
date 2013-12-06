program example(input, output);
var x: integer; var y: integer; var out: integer;
function gcd(a: integer; b: integer): integer;
begin
	if b = 0 then
		gcd := a
    else
    	gcd := gcd(b, a mod b)
end;
begin
    x := 10;
    y := 3;
    out := gcd(x, y)
end.