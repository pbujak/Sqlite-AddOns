--#include-orm-mappings "orm-symbols.xml"

create table access(
  id integer primary key,
  flags integer not null,
  value real
  );

create table units(
  id integer primary key,
  name text unique not null
  );

create table symbols(
  id integer primary key,
  name text unique not null,
  id_unit integer not null,
  foreign key(id_unit) references units(id) on delete no action
	);

create table shapes(
  id integer primary key,
  id_symbol integer not null,
  guid text,
  content text not null,
  foreign key(id_symbol) references symbols(id) on delete no action
  );
 

create index idx_shapes_id_symbol on shapes( id_symbol );
create index idx_symbols_id_unit on symbols( id_unit );

create view ALL_symbols( id, unit, name ) as
  select s.id, u.name, s.name
  from units u
  inner join symbols s on u.id=s.id_unit;

create view ALL_shapes( id, unit, symbol, guid, content ) as
 select sh.id, u.name, s.name, sh.guid, sh.content
 from units u
 inner join symbols s on u.id=s.id_unit
 inner join shapes sh on sh.id_symbol=s.id;
 
/* ------------------------------------------------------------------------------------
  Insert to all_shapes
*/
create trigger INSERT_all_shapes
instead of insert on all_shapes
for each row
when new.unit is not null and new.symbol is not null and new.content is not null
begin

insert or ignore into units( name )
values( new.unit );

insert or ignore into symbols( name, Id_unit)
values( new.symbol, ( select id from units where name = new.unit ) );

insert into shapes( guid, content, id_symbol )
values( new.guid, new.content, ( select id from symbols where name = new.symbol ) );

end;

/* ------------------------------------------------------------------------------------
  Delete from all_shapes
*/
create trigger DELETE_all_shapes
instead of delete on all_shapes
for each row
begin

delete from shapes
where id = old.id;

delete from symbols
where name = old.symbol
and not exists( select * from shapes where shapes.id_symbol = symbols.id );

delete from units
where name = old.unit
and not exists( select * from symbols where symbols.id_unit = units.id );

end;

/* ------------------------------------------------------------------------------------
  Update all_shapes
*/
create trigger UPDATE_all_shapes
instead of update of guid, content on all_shapes
for each row
begin

update shapes
set content = new.content, guid = new.guid
where id = old.id;

end;

/* ------------------------------------------------------------------------------------
  Unit Tests
*/
insert into ALL_shapes( unit, symbol, guid, content )
values( "Unit0", "Symbol0", "no guid", "Some content" );

set @lastId = select max( id ) from ALL_shapes;

assert ( select unit from ALL_shapes where id = @lastId ) = "Unit0";
assert ( select symbol from ALL_shapes where id = @lastId ) = "Symbol0";

update ALL_shapes
set guid = "GGG", content = "Some content"
where id = @lastId;

assert ( select guid from ALL_shapes where id = @lastId ) = "GGG";
assert ( select content from ALL_shapes where id = @lastId ) = "Some content";

delete from ALL_shapes;

