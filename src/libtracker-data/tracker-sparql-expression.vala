/*
 * Copyright (C) 2008-2010, Nokia
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

class Tracker.Sparql.Expression : Object {
	Query query;

	const string XSD_NS = "http://www.w3.org/2001/XMLSchema#";
	const string FN_NS = "http://www.w3.org/2005/xpath-functions#";
	const string FTS_NS = "http://www.tracker-project.org/ontologies/fts#";
	const string TRACKER_NS = "http://www.tracker-project.org/ontologies/tracker#";

	public Expression (Query query) {
		this.query = query;
	}

	Context context {
		get { return query.context; }
		set { query.context = value; }
	}

	Pattern pattern {
		get { return query.pattern; }
	}

	inline bool next () throws SparqlError {
		return query.next ();
	}

	inline SparqlTokenType current () {
		return query.current ();
	}

	inline SparqlTokenType last () {
		return query.last ();
	}

	inline bool accept (SparqlTokenType type) throws SparqlError {
		return query.accept (type);
	}

	SparqlError get_error (string msg) {
		return query.get_error (msg);
	}

	bool expect (SparqlTokenType type) throws SparqlError {
		return query.expect (type);
	}

	string get_last_string (int strip = 0) {
		return query.get_last_string (strip);
	}

	string escape_sql_string_literal (string literal) {
		return "'%s'".printf (string.joinv ("''", literal.split ("'")));
	}

	bool maybe_numeric (PropertyType type) {
		return (type == PropertyType.INTEGER || type == PropertyType.DOUBLE || type == PropertyType.DATETIME || type == PropertyType.UNKNOWN);
	}

	void skip_bracketted_expression () throws SparqlError {
		expect (SparqlTokenType.OPEN_PARENS);
		while (true) {
			switch (current ()) {
			case SparqlTokenType.OPEN_PARENS:
				// skip nested bracketted expression
				skip_bracketted_expression ();
				continue;
			case SparqlTokenType.CLOSE_PARENS:
			case SparqlTokenType.EOF:
				break;
			default:
				next ();
				continue;
			}
			break;
		}
		expect (SparqlTokenType.CLOSE_PARENS);
	}

	internal void skip_select_variables () throws SparqlError {
		while (true) {
			switch (current ()) {
			case SparqlTokenType.OPEN_PARENS:
				skip_bracketted_expression ();
				continue;
			case SparqlTokenType.FROM:
			case SparqlTokenType.WHERE:
			case SparqlTokenType.OPEN_BRACE:
			case SparqlTokenType.GROUP:
			case SparqlTokenType.ORDER:
			case SparqlTokenType.LIMIT:
			case SparqlTokenType.OFFSET:
			case SparqlTokenType.EOF:
				break;
			default:
				next ();
				continue;
			}
			break;
		}
	}

	internal PropertyType translate_select_expression (StringBuilder sql, bool subquery) throws SparqlError {
		Variable variable = null;

		long begin = sql.len;
		var type = PropertyType.UNKNOWN;
		if (accept (SparqlTokenType.COUNT)) {
			sql.append ("COUNT(");
			translate_aggregate_expression (sql);
			sql.append (")");
			type = PropertyType.INTEGER;
		} else if (accept (SparqlTokenType.SUM)) {
			sql.append ("SUM(");
			type = translate_aggregate_expression (sql);
			sql.append (")");
		} else if (accept (SparqlTokenType.AVG)) {
			sql.append ("AVG(");
			type = translate_aggregate_expression (sql);
			sql.append (")");
		} else if (accept (SparqlTokenType.MIN)) {
			sql.append ("MIN(");
			type = translate_aggregate_expression (sql);
			sql.append (")");
		} else if (accept (SparqlTokenType.MAX)) {
			sql.append ("MAX(");
			type = translate_aggregate_expression (sql);
			sql.append (")");
		} else if (accept (SparqlTokenType.GROUP_CONCAT)) {
			sql.append ("GROUP_CONCAT(");
			expect (SparqlTokenType.OPEN_PARENS);
			translate_expression_as_string (sql);
			sql.append (", ");
			expect (SparqlTokenType.COMMA);
			sql.append (escape_sql_string_literal (parse_string_literal ()));
			sql.append (")");
			expect (SparqlTokenType.CLOSE_PARENS);
			type = PropertyType.STRING;
		} else if (current () == SparqlTokenType.VAR) {
			type = translate_expression (sql);
			// we need variable name in case of compositional subqueries
			variable = context.get_variable (get_last_string ().substring (1));

			if (variable.binding == null) {
				throw get_error ("use of undefined variable `%s'".printf (variable.name));
			}
		} else {
			type = translate_expression (sql);
		}

		if (!subquery) {
			convert_expression_to_string (sql, type, begin);
			type = PropertyType.STRING;
		}

		if (accept (SparqlTokenType.AS)) {
			if (accept (SparqlTokenType.PN_PREFIX)) {
				// deprecated but supported for backward compatibility
				// (...) AS foo
				variable = context.get_variable (get_last_string ());
			} else {
				// syntax from SPARQL 1.1 Draft
				// (...) AS ?foo
				expect (SparqlTokenType.VAR);
				variable = context.get_variable (get_last_string ().substring (1));
			}
			sql.append_printf (" AS %s", variable.sql_expression);

			if (subquery) {
				var binding = new VariableBinding ();
				binding.data_type = type;
				binding.variable = variable;
				binding.sql_expression = variable.sql_expression;
				pattern.add_variable_binding (new StringBuilder (), binding, VariableState.BOUND);
			}
		}

		if (variable != null) {
			int state = context.var_set.lookup (variable);
			if (state == 0) {
				state = VariableState.BOUND;
			}
			context.select_var_set.insert (variable, state);
		}

		return type;
	}

	void translate_expression_as_order_condition (StringBuilder sql) throws SparqlError {
		long begin = sql.len;
		if (translate_expression (sql) == PropertyType.RESOURCE) {
			// ID => Uri
			sql.insert (begin, "(SELECT Uri FROM Resource WHERE ID = ");
			sql.append (")");
		}
	}

	internal void translate_order_condition (StringBuilder sql) throws SparqlError {
		if (accept (SparqlTokenType.ASC)) {
			translate_expression_as_order_condition (sql);
			sql.append (" ASC");
		} else if (accept (SparqlTokenType.DESC)) {
			translate_expression_as_order_condition (sql);
			sql.append (" DESC");
		} else {
			translate_expression_as_order_condition (sql);
		}
	}

	void translate_bound_call (StringBuilder sql) throws SparqlError {
		expect (SparqlTokenType.BOUND);
		expect (SparqlTokenType.OPEN_PARENS);
		sql.append ("(");
		translate_expression (sql);
		sql.append (" IS NOT NULL)");
		expect (SparqlTokenType.CLOSE_PARENS);
	}

	void translate_regex (StringBuilder sql) throws SparqlError {
		expect (SparqlTokenType.REGEX);
		expect (SparqlTokenType.OPEN_PARENS);
		sql.append ("SparqlRegex(");
		translate_expression_as_string (sql);
		sql.append (", ");
		expect (SparqlTokenType.COMMA);
		translate_expression (sql);
		sql.append (", ");
		if (accept (SparqlTokenType.COMMA)) {
			translate_expression (sql);
		} else {
			sql.append ("''");
		}
		sql.append (")");
		expect (SparqlTokenType.CLOSE_PARENS);
	}

	internal static void append_expression_as_string (StringBuilder sql, string expression, PropertyType type) {
		long begin = sql.len;
		sql.append (expression);
		convert_expression_to_string (sql, type, begin);
	}

	static void convert_expression_to_string (StringBuilder sql, PropertyType type, long begin) {
		switch (type) {
		case PropertyType.STRING:
		case PropertyType.INTEGER:
			// nothing to convert
			break;
		case PropertyType.RESOURCE:
			// ID => Uri
			sql.insert (begin, "(SELECT Uri FROM Resource WHERE ID = ");
			sql.append (")");
			break;
		case PropertyType.BOOLEAN:
			// 0/1 => false/true
			sql.insert (begin, "CASE ");
			sql.append (" WHEN 1 THEN 'true' WHEN 0 THEN 'false' ELSE NULL END");
			break;
		case PropertyType.DATETIME:
			// ISO 8601 format
			sql.insert (begin, "strftime (\"%Y-%m-%dT%H:%M:%SZ\", ");
			sql.append (", \"unixepoch\")");
			break;
		default:
			// let sqlite convert the expression to string
			sql.insert (begin, "CAST (");
			sql.append (" AS TEXT)");
			break;
		}
	}

	void translate_expression_as_string (StringBuilder sql) throws SparqlError {
		switch (current ()) {
		case SparqlTokenType.IRI_REF:
		case SparqlTokenType.PN_PREFIX:
		case SparqlTokenType.COLON:
			// handle IRI literals separately as it wouldn't work for unknown IRIs otherwise
			var binding = new LiteralBinding ();
			bool is_var;
			binding.literal = pattern.parse_var_or_term (null, out is_var);
			if (accept (SparqlTokenType.OPEN_PARENS)) {
				// function call
				long begin = sql.len;
				var type = translate_function (sql, binding.literal);
				expect (SparqlTokenType.CLOSE_PARENS);
				convert_expression_to_string (sql, type, begin);
			} else {
				sql.append ("?");
				query.bindings.append (binding);
			}
			break;
		default:
			long begin = sql.len;
			var type = translate_expression (sql);
			convert_expression_to_string (sql, type, begin);
			break;
		}
	}

	void translate_str (StringBuilder sql) throws SparqlError {
		expect (SparqlTokenType.STR);
		expect (SparqlTokenType.OPEN_PARENS);

		translate_expression_as_string (sql);

		expect (SparqlTokenType.CLOSE_PARENS);
	}

	void translate_isuri (StringBuilder sql) throws SparqlError {
		if (!accept (SparqlTokenType.ISURI)) {
			expect (SparqlTokenType.ISIRI);
		}

		expect (SparqlTokenType.OPEN_PARENS);

		sql.append ("?");
		var new_binding = new LiteralBinding ();
		new_binding.data_type = PropertyType.INTEGER;

		if (current() == SparqlTokenType.IRI_REF) {
			new_binding.literal = "1";
			next ();
		} else if (translate_expression (new StringBuilder ()) == PropertyType.RESOURCE) {
			new_binding.literal = "1";
		} else {
			new_binding.literal = "0";
		}

		query.bindings.append (new_binding);

		expect (SparqlTokenType.CLOSE_PARENS);
	}

	void translate_datatype (StringBuilder sql) throws SparqlError {
		expect (SparqlTokenType.DATATYPE);
		expect (SparqlTokenType.OPEN_PARENS);

		if (accept (SparqlTokenType.VAR)) {
			string variable_name = get_last_string().substring(1);
			var variable = context.get_variable (variable_name);

			if (variable.binding == null) {
				throw get_error ("`%s' is not a valid variable".printf (variable.name));
			}

			if (variable.binding.data_type == PropertyType.RESOURCE || variable.binding.type == null) {
				throw get_error ("Invalid FILTER");
			}

			sql.append ("(SELECT ID FROM Resource WHERE Uri = ?)");

			var new_binding = new LiteralBinding ();
			new_binding.literal = variable.binding.type.uri;
			query.bindings.append (new_binding);

		} else {
			throw get_error ("Invalid FILTER");
		}

		expect (SparqlTokenType.CLOSE_PARENS);
	}

	PropertyType translate_function (StringBuilder sql, string uri) throws SparqlError {
		if (uri == XSD_NS + "string") {
			// conversion to string
			translate_expression_as_string (sql);

			return PropertyType.STRING;
		} else if (uri == XSD_NS + "integer") {
			// conversion to integer
			sql.append ("CAST (");
			translate_expression_as_string (sql);
			sql.append (" AS INTEGER)");

			return PropertyType.INTEGER;
		} else if (uri == XSD_NS + "double") {
			// conversion to double
			sql.append ("CAST (");
			translate_expression_as_string (sql);
			sql.append (" AS REAL)");

			return PropertyType.DOUBLE;
		} else if (uri == FN_NS + "contains") {
			// fn:contains('A','B') => 'A' GLOB '*B*'
			sql.append ("(");
			translate_expression_as_string (sql);
			sql.append (" GLOB ");
			expect (SparqlTokenType.COMMA);

			sql.append ("?");
			var binding = new LiteralBinding ();
			binding.literal = "*%s*".printf (parse_string_literal ());
			query.bindings.append (binding);

			sql.append (")");

			return PropertyType.BOOLEAN;
		} else if (uri == FN_NS + "starts-with") {
			// fn:starts-with('A','B') => 'A' GLOB 'B*'
			sql.append ("(");
			translate_expression_as_string (sql);
			sql.append (" GLOB ");
			expect (SparqlTokenType.COMMA);

			sql.append ("?");
			var binding = new LiteralBinding ();
			binding.literal = "%s*".printf (parse_string_literal ());
			query.bindings.append (binding);

			sql.append (")");

			return PropertyType.BOOLEAN;
		} else if (uri == FN_NS + "ends-with") {
			// fn:ends-with('A','B') => 'A' GLOB '*B'
			sql.append ("(");
			translate_expression_as_string (sql);
			sql.append (" GLOB ");
			expect (SparqlTokenType.COMMA);

			sql.append ("?");
			var binding = new LiteralBinding ();
			binding.literal = "*%s".printf (parse_string_literal ());
			query.bindings.append (binding);

			sql.append (")");

			return PropertyType.BOOLEAN;
		} else if (uri == FN_NS + "concat") {
			translate_expression (sql);
			sql.append ("||");
			expect (SparqlTokenType.COMMA);
			translate_expression (sql);
			while (accept (SparqlTokenType.COMMA)) {
			      sql.append ("||");
			      translate_expression (sql);
			}

			return PropertyType.STRING;
		} else if (uri == FN_NS + "string-join") {
			sql.append ("SparqlStringJoin(");
			expect (SparqlTokenType.OPEN_PARENS);

			translate_expression_as_string (sql);
			sql.append (", ");
			expect (SparqlTokenType.COMMA);
			translate_expression_as_string (sql);
			while (accept (SparqlTokenType.COMMA)) {
			      sql.append (", ");
			      translate_expression_as_string (sql);
			}

			expect (SparqlTokenType.CLOSE_PARENS);
			sql.append (",");
			expect (SparqlTokenType.COMMA);
			translate_expression (sql);
			sql.append (")");

			return PropertyType.STRING;
		} else if (uri == FN_NS + "year-from-dateTime") {
			expect (SparqlTokenType.VAR);
			string variable_name = get_last_string ().substring (1);
			var variable = context.get_variable (variable_name);

			sql.append ("strftime (\"%Y\", ");
			sql.append (variable.get_extra_sql_expression ("localDate"));
			sql.append (" * 24 * 3600, \"unixepoch\")");

			return PropertyType.INTEGER;
		} else if (uri == FN_NS + "month-from-dateTime") {
			expect (SparqlTokenType.VAR);
			string variable_name = get_last_string ().substring (1);
			var variable = context.get_variable (variable_name);

			sql.append ("strftime (\"%m\", ");
			sql.append (variable.get_extra_sql_expression ("localDate"));
			sql.append (" * 24 * 3600, \"unixepoch\")");

			return PropertyType.INTEGER;
		} else if (uri == FN_NS + "day-from-dateTime") {
			expect (SparqlTokenType.VAR);
			string variable_name = get_last_string ().substring (1);
			var variable = context.get_variable (variable_name);

			sql.append ("strftime (\"%d\", ");
			sql.append (variable.get_extra_sql_expression ("localDate"));
			sql.append (" * 24 * 3600, \"unixepoch\")");

			return PropertyType.INTEGER;
		} else if (uri == FN_NS + "hours-from-dateTime") {
			expect (SparqlTokenType.VAR);
			string variable_name = get_last_string ().substring (1);
			var variable = context.get_variable (variable_name);

			sql.append ("(");
			sql.append (variable.get_extra_sql_expression ("localTime"));
			sql.append (" / 3600)");

			return PropertyType.INTEGER;
		} else if (uri == FN_NS + "minutes-from-dateTime") {
			expect (SparqlTokenType.VAR);
			string variable_name = get_last_string ().substring (1);
			var variable = context.get_variable (variable_name);

			sql.append ("(");
			sql.append (variable.get_extra_sql_expression ("localTime"));
			sql.append (" / 60 % 60)");

			return PropertyType.INTEGER;
		} else if (uri == FN_NS + "seconds-from-dateTime") {
			expect (SparqlTokenType.VAR);
			string variable_name = get_last_string ().substring (1);
			var variable = context.get_variable (variable_name);

			sql.append ("(");
			sql.append (variable.get_extra_sql_expression ("localTime"));
			sql.append ("% 60)");

			return PropertyType.INTEGER;
		} else if (uri == FN_NS + "timezone-from-dateTime") {
			expect (SparqlTokenType.VAR);
			string variable_name = get_last_string ().substring (1);
			var variable = context.get_variable (variable_name);

			sql.append ("(");
			sql.append (variable.get_extra_sql_expression ("localDate"));
			sql.append (" * 24 * 3600 + ");
			sql.append (variable.get_extra_sql_expression ("localTime"));
			sql.append ("- ");
			sql.append (variable.sql_expression);
			sql.append (")");

			return PropertyType.INTEGER;
		} else if (uri == FTS_NS + "rank") {
			bool is_var;
			string v = pattern.parse_var_or_term (null, out is_var);
			sql.append_printf ("\"%s_u_rank\"", v);

			return PropertyType.DOUBLE;
		} else if (uri == FTS_NS + "offsets") {
			bool is_var;
			string v = pattern.parse_var_or_term (null, out is_var);
			sql.append_printf ("\"%s_u_offsets\"", v);

			return PropertyType.STRING;
		} else if (uri == TRACKER_NS + "cartesian-distance") {
			sql.append ("SparqlCartesianDistance(");
			translate_expression (sql);
			sql.append (", ");
			expect (SparqlTokenType.COMMA);
			translate_expression (sql);
			sql.append (", ");
			expect (SparqlTokenType.COMMA);
			translate_expression (sql);
			sql.append (", ");
			expect (SparqlTokenType.COMMA);
			translate_expression (sql);
			sql.append (")");

			return PropertyType.DOUBLE;
		} else if (uri == TRACKER_NS + "haversine-distance") {
			sql.append ("SparqlHaversineDistance(");
			translate_expression (sql);
			sql.append (", ");
			expect (SparqlTokenType.COMMA);
			translate_expression (sql);
			sql.append (", ");
			expect (SparqlTokenType.COMMA);
			translate_expression (sql);
			sql.append (", ");
			expect (SparqlTokenType.COMMA);
			translate_expression (sql);
			sql.append (")");

			return PropertyType.DOUBLE;
		} else if (uri == TRACKER_NS + "coalesce") {
			sql.append ("COALESCE(");
			translate_expression_as_string (sql);
			sql.append (", ");
			expect (SparqlTokenType.COMMA);
			translate_expression_as_string (sql);
			while (accept (SparqlTokenType.COMMA)) {
			      sql.append (", ");
			      translate_expression_as_string (sql);
			}
			sql.append (")");

			return PropertyType.STRING;
		} else if (uri == TRACKER_NS + "string-from-filename") {
			sql.append ("SparqlStringFromFilename(");
			translate_expression_as_string (sql);
			sql.append (")");

			return PropertyType.STRING;
		} else {
			// support properties as functions
			var prop = Ontologies.get_property_by_uri (uri);
			if (prop == null) {
				throw get_error ("Unknown function");
			}

			if (prop.multiple_values) {
				sql.append ("(SELECT GROUP_CONCAT(");
				long begin = sql.len;
				sql.append_printf ("\"%s\"", prop.name);
				convert_expression_to_string (sql, prop.data_type, begin);
				sql.append_printf (",',') FROM \"%s\" WHERE ID = ", prop.table_name);
				translate_expression (sql);
				sql.append (")");

				return PropertyType.STRING;
			} else {
				sql.append_printf ("(SELECT \"%s\" FROM \"%s\" WHERE ID = ", prop.name, prop.table_name);
				translate_expression (sql);
				sql.append (")");

				return prop.data_type;
			}
		}
	}

	internal string parse_string_literal () throws SparqlError {
		next ();
		switch (last ()) {
		case SparqlTokenType.STRING_LITERAL1:
		case SparqlTokenType.STRING_LITERAL2:
			var sb = new StringBuilder ();

			string s = get_last_string (1);
			string* p = s;
			string* end = p + s.size ();
			while ((long) p < (long) end) {
				string* q = Posix.strchr (p, '\\');
				if (q == null) {
					sb.append_len (p, (long) (end - p));
					p = end;
				} else {
					sb.append_len (p, (long) (q - p));
					p = q + 1;
					switch (((char*) p)[0]) {
					case '\'':
					case '"':
					case '\\':
						sb.append_c (((char*) p)[0]);
						break;
					case 'b':
						sb.append_c ('\b');
						break;
					case 'f':
						sb.append_c ('\f');
						break;
					case 'n':
						sb.append_c ('\n');
						break;
					case 'r':
						sb.append_c ('\r');
						break;
					case 't':
						sb.append_c ('\t');
						break;
					}
					p++;
				}
			}

			if (accept (SparqlTokenType.DOUBLE_CIRCUMFLEX)) {
				if (!accept (SparqlTokenType.IRI_REF)) {
					accept (SparqlTokenType.PN_PREFIX);
					expect (SparqlTokenType.COLON);
				}
			}

			return sb.str;
		case SparqlTokenType.STRING_LITERAL_LONG1:
		case SparqlTokenType.STRING_LITERAL_LONG2:
			string result = get_last_string (3);

			if (accept (SparqlTokenType.DOUBLE_CIRCUMFLEX)) {
				if (!accept (SparqlTokenType.IRI_REF)) {
					accept (SparqlTokenType.PN_PREFIX);
					expect (SparqlTokenType.COLON);
				}
			}

			return result;
		default:
			throw get_error ("expected string literal");
		}
	}

	PropertyType translate_uri_expression (StringBuilder sql, string uri) throws SparqlError {
		if (accept (SparqlTokenType.OPEN_PARENS)) {
			// function
			var result = translate_function (sql, uri);
			expect (SparqlTokenType.CLOSE_PARENS);
			return result;
		} else {
			// resource
			sql.append ("(SELECT ID FROM Resource WHERE Uri = ?)");
			var binding = new LiteralBinding ();
			binding.literal = uri;
			query.bindings.append (binding);
			return PropertyType.RESOURCE;
		}
	}

	PropertyType translate_primary_expression (StringBuilder sql) throws SparqlError {
		switch (current ()) {
		case SparqlTokenType.OPEN_PARENS:
			return translate_bracketted_expression (sql);
		case SparqlTokenType.IRI_REF:
			next ();
			return translate_uri_expression (sql, get_last_string (1));
		case SparqlTokenType.DECIMAL:
		case SparqlTokenType.DOUBLE:
			next ();

			sql.append ("?");

			var binding = new LiteralBinding ();
			binding.literal = get_last_string ();
			query.bindings.append (binding);

			return PropertyType.DOUBLE;
		case SparqlTokenType.TRUE:
			next ();

			sql.append ("?");

			var binding = new LiteralBinding ();
			binding.literal = "1";
			binding.data_type = PropertyType.INTEGER;
			query.bindings.append (binding);

			return PropertyType.BOOLEAN;
		case SparqlTokenType.FALSE:
			next ();

			sql.append ("?");

			var binding = new LiteralBinding ();
			binding.literal = "0";
			binding.data_type = PropertyType.INTEGER;
			query.bindings.append (binding);

			return PropertyType.BOOLEAN;
		case SparqlTokenType.STRING_LITERAL1:
		case SparqlTokenType.STRING_LITERAL2:
		case SparqlTokenType.STRING_LITERAL_LONG1:
		case SparqlTokenType.STRING_LITERAL_LONG2:
			sql.append ("?");

			var binding = new LiteralBinding ();
			binding.literal = parse_string_literal ();
			query.bindings.append (binding);

			return PropertyType.STRING;
		case SparqlTokenType.INTEGER:
			next ();

			sql.append ("?");

			var binding = new LiteralBinding ();
			binding.literal = get_last_string ();
			binding.data_type = PropertyType.INTEGER;
			query.bindings.append (binding);

			return PropertyType.INTEGER;
		case SparqlTokenType.VAR:
			next ();
			string variable_name = get_last_string ().substring (1);
			var variable = context.get_variable (variable_name);
			sql.append (variable.sql_expression);

			if (variable.binding == null) {
				return PropertyType.UNKNOWN;
			} else {
				return variable.binding.data_type;
			}
		case SparqlTokenType.STR:
			translate_str (sql);
			return PropertyType.STRING;
		case SparqlTokenType.LANG:
			next ();
			sql.append ("''");
			return PropertyType.STRING;
		case SparqlTokenType.LANGMATCHES:
			next ();
			sql.append ("0");
			return PropertyType.BOOLEAN;
		case SparqlTokenType.DATATYPE:
			translate_datatype (sql);
			return PropertyType.RESOURCE;
		case SparqlTokenType.BOUND:
			translate_bound_call (sql);
			return PropertyType.BOOLEAN;
		case SparqlTokenType.SAMETERM:
			next ();
			expect (SparqlTokenType.OPEN_PARENS);
			sql.append ("(");
			translate_expression (sql);
			sql.append (" = ");
			expect (SparqlTokenType.COMMA);
			translate_expression (sql);
			sql.append (")");
			expect (SparqlTokenType.CLOSE_PARENS);
			return PropertyType.BOOLEAN;
		case SparqlTokenType.ISIRI:
		case SparqlTokenType.ISURI:
			translate_isuri (sql);
			return PropertyType.BOOLEAN;
		case SparqlTokenType.ISBLANK:
			next ();
			expect (SparqlTokenType.OPEN_PARENS);
			next ();
			// TODO: support ISBLANK properly
			sql.append ("0");
			expect (SparqlTokenType.CLOSE_PARENS);
			return PropertyType.BOOLEAN;
		case SparqlTokenType.ISLITERAL:
			next ();
			return PropertyType.BOOLEAN;
		case SparqlTokenType.REGEX:
			translate_regex (sql);
			return PropertyType.BOOLEAN;
		case SparqlTokenType.PN_PREFIX:
			next ();
			string ns = get_last_string ();
			expect (SparqlTokenType.COLON);
			string uri = query.resolve_prefixed_name (ns, get_last_string ().substring (1));
			return translate_uri_expression (sql, uri);
		case SparqlTokenType.COLON:
			next ();
			string uri = query.resolve_prefixed_name ("", get_last_string ().substring (1));
			return translate_uri_expression (sql, uri);
		default:
			throw get_error ("expected primary expression");
		}
	}

	PropertyType translate_unary_expression (StringBuilder sql) throws SparqlError {
		if (accept (SparqlTokenType.OP_NEG)) {
			sql.append ("NOT (");
			var optype = translate_primary_expression (sql);
			sql.append (")");
			if (optype != PropertyType.BOOLEAN) {
				throw get_error ("expected boolean expression");
			}
			return PropertyType.BOOLEAN;
		} else if (accept (SparqlTokenType.PLUS)) {
			return translate_primary_expression (sql);
		} else if (accept (SparqlTokenType.MINUS)) {
			sql.append ("-(");
			var optype = translate_primary_expression (sql);
			sql.append (")");
			return optype;
		}
		return translate_primary_expression (sql);
	}

	PropertyType translate_multiplicative_expression (StringBuilder sql) throws SparqlError {
		long begin = sql.len;
		var optype = translate_unary_expression (sql);
		while (true) {
			if (accept (SparqlTokenType.STAR)) {
				if (!maybe_numeric (optype)) {
					throw get_error ("expected numeric operand");
				}
				sql.insert (begin, "(");
				sql.append (" * ");
				if (!maybe_numeric (translate_unary_expression (sql))) {
					throw get_error ("expected numeric operand");
				}
				sql.append (")");
			} else if (accept (SparqlTokenType.DIV)) {
				if (!maybe_numeric (optype)) {
					throw get_error ("expected numeric operand");
				}
				sql.insert (begin, "(");
				sql.append (" / ");
				if (!maybe_numeric (translate_unary_expression (sql))) {
					throw get_error ("expected numeric operand");
				}
				sql.append (")");
			} else {
				break;
			}
		}
		return optype;
	}

	PropertyType translate_additive_expression (StringBuilder sql) throws SparqlError {
		long begin = sql.len;
		var optype = translate_multiplicative_expression (sql);
		while (true) {
			if (accept (SparqlTokenType.PLUS)) {
				if (!maybe_numeric (optype)) {
					throw get_error ("expected numeric operand");
				}
				sql.insert (begin, "(");
				sql.append (" + ");
				if (!maybe_numeric (translate_multiplicative_expression (sql))) {
					throw get_error ("expected numeric operand");
				}
				sql.append (")");
			} else if (accept (SparqlTokenType.MINUS)) {
				if (!maybe_numeric (optype)) {
					throw get_error ("expected numeric operand");
				}
				sql.insert (begin, "(");
				sql.append (" - ");
				if (!maybe_numeric (translate_multiplicative_expression (sql))) {
					throw get_error ("expected numeric operand");
				}
				sql.append (")");
			} else {
				break;
			}
		}
		return optype;
	}

	PropertyType translate_numeric_expression (StringBuilder sql) throws SparqlError {
		return translate_additive_expression (sql);
	}

	PropertyType process_relational_expression (StringBuilder sql, long begin, uint n_bindings, PropertyType op1type, string operator) throws SparqlError {
		sql.insert (begin, "(");
		sql.append (operator);
		var op2type = translate_numeric_expression (sql);
		sql.append (")");
		if ((op1type == PropertyType.DATETIME && op2type == PropertyType.STRING)
		    || (op1type == PropertyType.STRING && op2type == PropertyType.DATETIME)) {
			// TODO: improve performance (linked list)
			if (query.bindings.length () == n_bindings + 1) {
				// trigger string => datetime conversion
				query.bindings.last ().data.data_type = PropertyType.DATETIME;
			}
		}
		return PropertyType.BOOLEAN;
	}

	PropertyType translate_relational_expression (StringBuilder sql) throws SparqlError {
		long begin = sql.len;
		// TODO: improve performance (linked list)
		uint n_bindings = query.bindings.length ();
		var optype = translate_numeric_expression (sql);
		if (accept (SparqlTokenType.OP_GE)) {
			return process_relational_expression (sql, begin, n_bindings, optype, " >= ");
		} else if (accept (SparqlTokenType.OP_EQ)) {
			return process_relational_expression (sql, begin, n_bindings, optype, " = ");
		} else if (accept (SparqlTokenType.OP_NE)) {
			return process_relational_expression (sql, begin, n_bindings, optype, " <> ");
		} else if (accept (SparqlTokenType.OP_LT)) {
			return process_relational_expression (sql, begin, n_bindings, optype, " < ");
		} else if (accept (SparqlTokenType.OP_LE)) {
			return process_relational_expression (sql, begin, n_bindings, optype, " <= ");
		} else if (accept (SparqlTokenType.OP_GT)) {
			return process_relational_expression (sql, begin, n_bindings, optype, " > ");
		}
		return optype;
	}

	PropertyType translate_value_logical (StringBuilder sql) throws SparqlError {
		return translate_relational_expression (sql);
	}

	PropertyType translate_conditional_and_expression (StringBuilder sql) throws SparqlError {
		long begin = sql.len;
		var optype = translate_value_logical (sql);
		while (accept (SparqlTokenType.OP_AND)) {
			if (optype != PropertyType.BOOLEAN) {
				throw get_error ("expected boolean expression");
			}
			sql.insert (begin, "(");
			sql.append (" AND ");
			optype = translate_value_logical (sql);
			sql.append (")");
			if (optype != PropertyType.BOOLEAN) {
				throw get_error ("expected boolean expression");
			}
		}
		return optype;
	}

	PropertyType translate_conditional_or_expression (StringBuilder sql) throws SparqlError {
		long begin = sql.len;
		var optype = translate_conditional_and_expression (sql);
		while (accept (SparqlTokenType.OP_OR)) {
			if (optype != PropertyType.BOOLEAN) {
				throw get_error ("expected boolean expression");
			}
			sql.insert (begin, "(");
			sql.append (" OR ");
			optype = translate_conditional_and_expression (sql);
			sql.append (")");
			if (optype != PropertyType.BOOLEAN) {
				throw get_error ("expected boolean expression");
			}
		}
		return optype;
	}

	internal PropertyType translate_expression (StringBuilder sql) throws SparqlError {
		return translate_conditional_or_expression (sql);
	}

	PropertyType translate_bracketted_expression (StringBuilder sql) throws SparqlError {
		expect (SparqlTokenType.OPEN_PARENS);

		if (current () == SparqlTokenType.SELECT) {
			// scalar subquery

			context = new Context.subquery (context);

			sql.append ("(");
			var type = pattern.translate_select (sql, true);
			sql.append (")");

			context = context.parent_context;

			expect (SparqlTokenType.CLOSE_PARENS);
			return type;
		}

		var optype = translate_expression (sql);
		expect (SparqlTokenType.CLOSE_PARENS);
		return optype;
	}

	PropertyType translate_aggregate_expression (StringBuilder sql) throws SparqlError {
		expect (SparqlTokenType.OPEN_PARENS);
		if (accept (SparqlTokenType.DISTINCT)) {
			sql.append ("DISTINCT ");
		}
		var optype = translate_expression (sql);
		expect (SparqlTokenType.CLOSE_PARENS);
		return optype;
	}

	internal PropertyType translate_constraint (StringBuilder sql) throws SparqlError {
		switch (current ()) {
		case SparqlTokenType.STR:
		case SparqlTokenType.LANG:
		case SparqlTokenType.LANGMATCHES:
		case SparqlTokenType.DATATYPE:
		case SparqlTokenType.BOUND:
		case SparqlTokenType.SAMETERM:
		case SparqlTokenType.ISIRI:
		case SparqlTokenType.ISURI:
		case SparqlTokenType.ISBLANK:
		case SparqlTokenType.ISLITERAL:
		case SparqlTokenType.REGEX:
			return translate_primary_expression (sql);
		default:
			return translate_bracketted_expression (sql);
		}
	}
}
