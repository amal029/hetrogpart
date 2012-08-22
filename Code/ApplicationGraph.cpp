// Heterogeneous Architecture GRaph Partitioner --- 1.0.0.1, Aug 2012
// 20.08.2012

/** @file ApplicationGraph.cpp
 *
 */

#ifndef APPLICATION_GRAPH_CPP
#define APPLICATION_GRAPH_CPP

#include "ApplicationGraph.hpp"

/*
 *  @brief Contains actual code of respective functions
 *         present in the header.
 *
 *  No detailed comments will be present, refer to the header
 *  for any documentation regarding the class and its inherent
 *  functions and attributes.
 */

//ApplicationGraph()
ApplicationGraph :: ApplicationGraph()
{

}
//end of ApplicationGraph()

//ReadGraph( char* file_name )
int ApplicationGraph :: ReadGraph( char* file_name )
{
	ifstream in( file_name );

	dynamic_properties dp;

	dp.property( "name", get( &AppNode::name, app_graph ) );
	dp.property( "label", get( &AppNode::label, app_graph ) );
	dp.property( "type", get( &AppNode::type, app_graph ) );
	dp.property( "constraints", get( &AppNode::constraints, app_graph ) );
	dp.property( "shape", get( &AppNode::shape, app_graph ) );
	dp.property( "weight", get( &AppEdge::weight, app_graph ) );
	dp.property( "label", get( &AppEdge::label, app_graph ) );
	dp.property( "id", get( &AppNode::id, app_graph ) );

	bool status = read_graphviz( in, app_graph, dp, "name" );
	in.close();

	ParseTokens();

	PrintGraphViz( "app_graph" );

	return status;
}
//end of ReadGraph( char* file_name )

//ParseTokens()
void ApplicationGraph :: ParseTokens()
{
	const string& delimiters = ",";

	uint32_t max_no_constraints = 0;
	BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
	{
		// Skip delimiters at beginning.
		string::size_type lastPos = app_graph[ v ].constraints.find_first_not_of( delimiters, 0 );

		// Find first "non-delimiter".
		string::size_type pos = app_graph[ v ].constraints.find_first_of( delimiters, lastPos );

		while( string::npos != pos || string::npos != lastPos )
		{
			// Found a token, add it to the vector.
			app_graph[ v ].constraint.push_back(  lexical_cast< float_t > ( app_graph[ v ].constraints.substr( lastPos, pos - lastPos ) ) );

			// Skip delimiters.  Note the "not_of"
			lastPos = app_graph[ v ].constraints.find_first_not_of( delimiters, pos );

			// Find next "non-delimiter"
			pos = app_graph[ v ].constraints.find_first_of( delimiters, lastPos );
		}

		if( app_graph[ v ].constraint.size() > max_no_constraints )
			max_no_constraints = app_graph[ v ].constraint.size();
	}

	/*
	 * Provide uniform no of constraints for all nodes in the app graph
	 * If no of constraints is less then pad the rest with 0
	 */
	uint32_t index = 0;
	BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
	{
		app_graph[ v ].id = index;
		if( app_graph[ v ].constraint.size() < max_no_constraints )
		{
			for( uint32_t i = 0; i < max_no_constraints; i++ )
				app_graph[ v ].constraint.push_back( 0 );
		}
		index++;
	}

	BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
	{
		if( app_graph[ v ].name == "EmptyActor1" )
		{
			for( uint32_t i = 1; i < max_no_constraints; i++ )
				app_graph[ v ].constraint[ i ] = 1;

			break;
		}
	}

	/*
	BGL_FORALL_VERTICES( v, app_graph, ApplicationGraphType )
	{
		cout << app_graph[ v ].id << ":";
		for( uint32_t i = 0; i < app_graph[ v ].constraint.size(); i++ )
			cout << ";" << app_graph[ v ].constraint[ i ];
		cout << endl;
	}
	*/
}
//end of ParseTokens()

//PrintGraphViz( const char* output_file )
int ApplicationGraph :: PrintGraphViz( const char* output_file )
{
	dynamic_properties dp;

	dp.property( "name", get( &AppNode::name, app_graph ) );
	dp.property( "label", get( &AppNode::label, app_graph ) );
	dp.property( "type", get( &AppNode::type, app_graph ) );
	dp.property( "constraints", get( &AppNode::constraints, app_graph ) );
	dp.property( "shape", get( &AppNode::shape, app_graph ) );
	dp.property( "weight", get( &AppEdge::weight, app_graph ) );
	dp.property( "label", get( &AppEdge::label, app_graph ) );
	dp.property( "id", get( &AppNode::id, app_graph ) );

	ofstream out( output_file );
	write_graphviz_dp( out, app_graph, dp, string(), get( &AppNode::id, app_graph ) );
	out.close();

	return 0;
}
//end of PrintGraphViz( const char* output_file )

#endif
