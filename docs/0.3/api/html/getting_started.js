var getting_started =
[
    [ "Building and Installing AgentXcpp", "installation.html", [
      [ "Prerequisites", "installation.html#installation_prerequisites", null ],
      [ "Building and installing agentXcpp", "installation.html#installation_installingSystem", null ],
      [ "Uninstalling agentXcpp", "installation.html#installation_cleaning", null ]
    ] ],
    [ "How to write a Subagent", "how_to_write_a_subagent.html", [
      [ "Implementing the simpleCounter variable", "how_to_write_a_subagent.html#implement_class", null ],
      [ "Implementing the main() function", "how_to_write_a_subagent.html#main_function", null ],
      [ "Compiling the Subagent", "how_to_write_a_subagent.html#compiling", null ],
      [ "Running the Subagent", "how_to_write_a_subagent.html#running", [
        [ "Starting the Net-SNMP master agent", "how_to_write_a_subagent.html#starting_master", null ],
        [ "Starting the Subagent", "how_to_write_a_subagent.html#starting_subagent", null ],
        [ "Querying the Subagent", "how_to_write_a_subagent.html#querying", null ]
      ] ]
    ] ],
    [ "How to implement a writeable variable", "how_to_add_rw_support.html", [
      [ "The anatomy of a Set request", "how_to_add_rw_support.html#the_set_steps", null ],
      [ "Extending the SimpleCounter class", "how_to_add_rw_support.html#extend_simplecounter", [
        [ "TestSet", "how_to_add_rw_support.html#TestSet", null ],
        [ "CommitSet", "how_to_add_rw_support.html#CommitSet", null ],
        [ "CleanupSet", "how_to_add_rw_support.html#CleanupSet", null ],
        [ "UndoSet", "how_to_add_rw_support.html#UndoSet", null ]
      ] ],
      [ "Testing the extended subagent", "how_to_add_rw_support.html#try_it_out", null ]
    ] ],
    [ "How to send notifications and traps", "how_to_send_notifications.html", [
      [ "The Implementation", "how_to_send_notifications.html#implementing_notification", null ],
      [ "Compiling the Subagent", "how_to_send_notifications.html#compiling_notification", null ],
      [ "Running the Subagent", "how_to_send_notifications.html#running_notification", [
        [ "Configuring the master agent", "how_to_send_notifications.html#configuring_the_master_agent", null ],
        [ "Starting the snmptrapd program", "how_to_send_notifications.html#starting_snmptrapd", null ],
        [ "Starting the Subagent", "how_to_send_notifications.html#notifications_starting_subagent", null ]
      ] ]
    ] ],
    [ "How to implement a table", "how_to_implement_tables.html", [
      [ "How tables work in agentXcpp", "how_to_implement_tables.html#how_tables_work", null ],
      [ "Implementing the CounterChangeEntry class", "how_to_implement_tables.html#tables_implementing_entry", null ],
      [ "Extending the SimpleCounter class.", "how_to_implement_tables.html#tables_extending_simplecounter", null ],
      [ "Compiling the Subagent", "how_to_implement_tables.html#tables_compiling_notification", null ],
      [ "Running the subagent", "how_to_implement_tables.html#tables_running", null ]
    ] ]
];