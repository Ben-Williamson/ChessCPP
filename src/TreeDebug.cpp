
#include "TreeDebug.h"

//<?xml version="1.0" encoding="UTF-8"?>
//<gexf xmlns="http://www.gexf.net/1.3" version="1.3">
//  <meta lastmodifieddate="2025-05-18">
//    <creator>YourEngine 0.1</creator>
//    <description>Search tree d=4, AB pruning</description>
//  </meta>
//  <graph mode="static" defaultedgetype="directed">
//    <attributes class="node">
//      <attribute id="0" title="fen" type="string"/>
//      <attribute id="1" title="eval" type="float"/>
//      <attribute id="2" title="depth" type="integer"/>
//      <attribute id="3" title="cut" type="boolean"/>
//    </attributes>
//    <nodes>
//      <node id="0" label="root">
//        <attvalues>
//          <attvalue for="0" value="startpos"/>
//        </attvalues>
//      </node>
//      <node id="1" label="a3">
//        <attvalues>
//          <attvalue for="0" value="rnbqkbnr/pppppppp/..." />
//          <attvalue for="1" value="0.15"/>
//          <attvalue for="2" value="1"/>
//        </attvalues>
//      </node>
//    </nodes>
//    <edges>
//      <edge id="0" source="0" target="1" label="a2a3"/>
//    </edges>
//  </graph>
//</gexf>

TreeDebug::TreeDebug() {
    output_file = std::ofstream("../tree.gexf");
}

TreeDebug::~TreeDebug() {
    if (output_file.is_open()) {
        WriteHeaders();
        WriteNodes();
        WriteEdges();
        WriteFooter();
        output_file.close();
    }
}

void TreeDebug::WriteHeaders() {
    output_file <<
    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<gexf xmlns=\"http://www.gexf.net/1.3\" version=\"1.3\">\n"
    "  <meta lastmodifieddate=\"2025-05-18\">\n"
    "    <creator>BENgine 0.1</creator>\n"
    "    <description>Search tree</description>\n"
    "  </meta>\n"
    "  <graph mode=\"static\" defaultedgetype=\"directed\">\n"
    "    <attributes class=\"node\">\n"
    "      <attribute id=\"0\" title=\"label\" type=\"string\"/>\n"
    "    </attributes>\n";
}

void TreeDebug::WriteNodes() {
    output_file << "    <nodes>\n";

    for (int i = 0; i < nodes.size(); i++) {
        output_file <<
        "      <node id=\"" << i << "\" label=\"" << nodes[i].label << "\">\n"
        "        <attvalues>\n"
        "          <attvalue for=\"0\" value=\"" << nodes[i].label << "\"/>\n"
        "        </attvalues>\n"
        "      </node>\n";
    }
    output_file << "    </nodes>\n";
}

void TreeDebug::WriteEdges() {
    output_file << "    <edges>\n";
    for (int i = 0; i < edges.size(); i++) {
        output_file <<
        "      <edge id=\"" << i <<
        "\" source=\""<< edges[i].source <<
        "\" target=\"" << edges[i].target <<
        "\"/>\n";
    }
    output_file << "    </edges>\n";
}

void TreeDebug::WriteFooter() {
    output_file << "  </graph>\n" <<
    "</gexf>";
}

int TreeDebug::AddNode(std::string label) {
    nodes.push_back(
            Node{label}
            );
    return nodes.size() - 1;
}
void TreeDebug::AddEdge(int source, int target) {
    edges.push_back(
            Edge{source, target}
            );
}
