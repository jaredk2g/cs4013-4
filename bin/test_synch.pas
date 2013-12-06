program example();
var x: integer; var y: integer;
function gcd(a: ; b: integer): integer;
begin
	if b = 0 then
		gcd := a
    else
    	gcd := gcd(b, a mod b)
end;
begin
    x := read
    y := write
end.