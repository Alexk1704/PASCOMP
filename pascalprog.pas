program helloworld;

function test(c: integer; b: integer): integer;

var
	p: integer;
	
begin
	if(b > c) then
		b := b + 1
	else
		while c > b do
		begin
			c := c + 1;
		end;
	test := c + b / p;
	
end;

function testc(x: integer; y: integer): integer;

var
	g: integer;
	
begin
	if(x > y) then
		b := x + 1
	else
		c := y + 1;
	test := c + b * p;
	
end;
   
var
	x: real;
    b, c: integer;
    g: integer;
    p: integer;
    
begin
	b := 10;
	c := 10;
	c := test(test(c, b), test(test(c, b), b));
	b := testc(b, c);
	b := c;
end.
