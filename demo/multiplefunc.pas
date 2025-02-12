program mulfuncs;

type

	my_int = integer;

function func_a(n:integer; b: integer): integer;

begin
	if (n <= b) then
		func_a := func_b(n, b)
	else
		func_a := func_b(n, 5);
end;

function func_b(n:integer; a: integer): integer;

var
	i: integer;
	
begin
	func_b := n * a / i;

end;



var
	i, x : integer;
	c: char;
	my_i: my_int;

	
begin
	i := 1;
	while(i < 20) do
	begin
		my_i := 20;
		x := my_i + x;
		i := i + i * 2;
		x := func_a(i, func_a(func_a(10, i), 5));
	end;	
end.
