#!/usr/local/bin/perl -w
# bld.pl - executed from bld.sh - stdin is list of README paths

use strict;
use Getopt::Std;

my $main_index_file = open_main_index();
my $c_index_file = open_c_index();
my $java_index_file = open_java_index();
my $dotnet_index_file = open_dotnet_index();

# Main loop; read each line in each file.
while (<>) {
  chomp;  # remove trailing \n
  my ($t, $f) = split /\//;

  my ($summary, $description) = read_readme("$t/$f");

  # construct example line
  my ($c, $java, $dotnet) = (non_button("C"), non_button("Java"), non_button("C#/.NET"));
  if (file_found("$t/c/$t.sldoc")) {
    $c = mk_button("C", "$t/c/index.html");
  }
  if (file_found("$t/java/$t.sldoc")) {
    $java = mk_button("Java", "$t/java/index.html");
  }
  if (file_found("$t/dotnet/$t.sldoc")) {
    $dotnet = mk_button("C#/.NET", "$t/dotnet/index.html");
  }
  my $example_line = "<tr><td align=\"right\"><b>$t</b></td><td>$c&nbsp;$java&nbsp;$dotnet</td><td>$summary</td></tr>\n";

  print $main_index_file $example_line;

  # Conditionally add to language-specific example indicies.
  if (file_found("$t/c/$t.sldoc")) {
    print $c_index_file $example_line;
  }
  if (file_found("$t/java/$t.sldoc")) {
    print $java_index_file $example_line;
  }
  if (file_found("$t/dotnet/$t.sldoc")) {
    print $dotnet_index_file $example_line;
  }

  # Add to example-specific index.
  my $example_index_file = open_example_index($t, $summary, $description);
  $example_line =~ s/href=\"$t\//href=\"/g;  # index is already under the tool dir
  print $example_index_file $example_line;
  close_example_index($example_index_file);
}  # while

# All done.
close_main_index($main_index_file);
close_c_index($c_index_file);
close_java_index($java_index_file);
close_dotnet_index($dotnet_index_file);

exit(0);


sub file_found {
  my ($file_name) = @_;

  my $file;
  my $found = 0;
  if (open($file, "<", "$file_name")) {
    close($file);
    $found = 1;
  }
  return $found;
}  # file_found


sub non_button {
  my ($txt) = @_;

  return "<span class=\"nonbutton\">$txt</span>";
}  # non_button


sub mk_button {
  my ($txt, $link) = @_;

  return "<a href=\"$link\" class=\"button\" target=\"_top\">$txt</a>";
}  # mk_button


sub read_readme {
  my ($file_name) = @_;

  my $readme_file;
  open($readme_file, "<", "$file_name") || die("bld.pl: ERROR: read_readme: $file_name: could not open ($!)");

  my ($i, $summary, $description);

  while (defined($i = <$readme_file>)) {
    chomp($i);  # remove trailing \n
    # delete comments, leading spaces, trailing spaces
    $i =~ s/#.*$//;  $i =~ s/^\s*//;  $i =~ s/\s*$//;
    if ($i =~ /^\s*$/) { next; }  # skip blank lines

    # Parse out the summary and description
    my ($k, $v) = split /\s*=\s*/, $i;
    if ($k =~ /summary/i) {  # i = case-insensitive
      $summary = $v;
    } elsif ($k =~ /description/i) {  # i = case-insensitive
      $description = $v;
    } else {
      die("bld.pl: ERROR: read_readme: $file_name: bad input line: '$i'");
    }
  }  # while

  close($readme_file);

  if (! defined($summary)) {
    print STDERR "bld.pl: ERROR: read_readme: $file_name: summary not found\n";
    exit(1);
  }

  return ($summary, $description);
}  # read_readme


sub open_main_index {
  my ($file_name, $main_index_file) = ("index.html");
  open($main_index_file, ">", $file_name) || die("bld.pl: ERROR: open_main_index: $file_name: could not open ($!)");

  my $main_button = mk_button("All examples", "index.html");
  my $c_button = mk_button("C examples", "c_index.html");
  my $java_button = mk_button("Java examples", "java_index.html");
  my $dotnet_button = mk_button("C#/.NET examples", "dotnet_index.html");

  print $main_index_file <<__EOF__;
<html>
<head>
<title>UMExamples - Learning the Ultra Messaging API by example</title>
<link rel="stylesheet" href="style/main.css" type="text/css" media="screen" />
</head>
<body>
<h2>Welcome to UMExamples</h2>

<p>Ultra Messaging Examples (UMExamples) is a documentation suite aimed at
providing high quality educational samples for the Informatica Ultra Messaging
(UM) product line. Every sample in the suite is coded ready to compile, though
may not act as a great application as is. These examples are provided to
instruct users of the UM API on how to use the API and best practices with
regard to UM functionality. Along with each sample application is detailed
documentation describing the purpose of the example, and walks through the
sample code to provide clear instruction on how to best use Ultra Messaging.</p>

<p>In the list below, different examples are available in different source code
languages (C, Java, C#/.NET), but this site is a work in progress, so many
examples are not available in all laguages.</p>

<p>If you would like to see only those examples available in a particular language,
you can visit: $c_button $java_button $dotnet_button</p>

<p>The master list is always available at: $main_button</p>

<p>UMExamples is created with <a href="https://github.com/fordsfords/semlit">SEMLIT</a></p>

<h2>Examples</h2>
<table>
__EOF__

  return $main_index_file;
}  # open_main_index


sub close_main_index {
  my ($main_index_file) = @_;

  print $main_index_file <<__EOF__;
</table>
</body>
</html>
__EOF__

  close($main_index_file);
}  # close_main_index


sub open_c_index {
  my ($file_name, $c_index_file) = ("c_index.html");
  open($c_index_file, ">", $file_name) || die("bld.pl: ERROR: open_c_index: $file_name: could not open ($!)");

  my $main_button = mk_button("All examples", "index.html");
  my $c_button = mk_button("C examples", "c_index.html");
  my $java_button = mk_button("Java examples", "java_index.html");
  my $dotnet_button = mk_button("C#/.NET examples", "dotnet_index.html");

  print $c_index_file <<__EOF__;
<html>
<head>
<title>UMExamples - C Language</title>
<link rel="stylesheet" href="style/main.css" type="text/css" media="screen" />
</head>
<body>
<h2>C Examples</h2>
<p>Examples written in C.</p>

<p>In the list below, different examples are available in different source code
languages (C, Java, C#/.NET), but this site is a work in progress, so many
examples are not available in all laguages.</p>

<p>If you would like to see only those examples available in a particular language,
you can visit: $c_button $java_button $dotnet_button</p>

<p>The master list is always available at: $main_button</p>

<h2>Examples</h2>
<table>
__EOF__

  return $c_index_file;
}  # open_c_index


sub close_c_index {
  my ($c_index_file) = @_;

  print $c_index_file <<__EOF__;
</table>
</body>
</html>
__EOF__

  close($c_index_file);
}  # close_c_index


sub open_java_index {
  my ($file_name, $java_index_file) = ("java_index.html");
  open($java_index_file, ">", $file_name) || die("bld.pl: ERROR: open_java_index: $file_name: could not open ($!)");

  my $main_button = mk_button("All examples", "index.html");
  my $c_button = mk_button("C examples", "c_index.html");
  my $java_button = mk_button("Java examples", "java_index.html");
  my $dotnet_button = mk_button("C#/.NET examples", "dotnet_index.html");

  print $java_index_file <<__EOF__;
<html>
<head>
<title>UMExamples - Java Language</title>
<link rel="stylesheet" href="style/main.css" type="text/css" media="screen" />
</head>
<body>
<h2>Java Examples</h2>
<p>Examples written in Java.</p>

<p>In the list below, different examples are available in different source code
languages (C, Java, C#/.NET), but this site is a work in progress, so many
examples are not available in all laguages.</p>

<p>If you would like to see only those examples available in a particular language,
you can visit: $c_button $java_button $dotnet_button</p>

<p>The master list is always available at: $main_button</p>

<h2>Examples</h2>
<table>
__EOF__

  return $java_index_file;
}  # open_java_index


sub close_java_index {
  my ($java_index_file) = @_;

  print $java_index_file <<__EOF__;
</table>
</body>
</html>
__EOF__

  close($java_index_file);
}  # close_java_index


sub open_dotnet_index {
  my ($file_name, $dotnet_index_file) = ("dotnet_index.html");
  open($dotnet_index_file, ">", $file_name) || die("bld.pl: ERROR: open_dotnet_index: $file_name: could not open ($!)");

  my $main_button = mk_button("All examples", "index.html");
  my $c_button = mk_button("C examples", "c_index.html");
  my $java_button = mk_button("Java examples", "java_index.html");
  my $dotnet_button = mk_button("C#/.NET examples", "dotnet_index.html");

  print $dotnet_index_file <<__EOF__;
<html>
<head>
<title>UMExamples - C#/.NET Language</title>
<link rel="stylesheet" href="style/main.css" type="text/css" media="screen" />
</head>
<body>
<h2>C#/.NET Examples</h2>
<p>Examples written in C#/.NET.</p>

<p>In the list below, different examples are available in different source code
languages (C, Java, C#/.NET), but this site is a work in progress, so many
examples are not available in all laguages.</p>

<p>If you would like to see only those examples available in a particular language,
you can visit: $c_button $java_button $dotnet_button</p>

<p>The master list is always available at: $main_button</p>

<h2>Examples</h2>
<table>
__EOF__

  return $dotnet_index_file;
}  # open_dotnet_index


sub close_dotnet_index {
  my ($dotnet_index_file) = @_;

  print $dotnet_index_file <<__EOF__;
</table>
</body>
</html>
__EOF__

  close($dotnet_index_file);
}  # close_dotnet_index


sub open_example_index {
  my ($example, $summary, $description) = @_;

  my ($file_name, $example_index_file) = ("$example/index.html");
  open($example_index_file, ">", $file_name) || die("bld.pl: ERROR: open_example_index: $file_name: could not open ($!)");

  my $main_button = mk_button("All examples", "index.html");

  print $example_index_file <<__EOF__;
<html>
<head>
<title>UMExamples - $example</title>
<link rel="stylesheet" href="../style/main.css" type="text/css" media="screen" />
</head>
<body>
<p>The master list of examples is available at: $main_button</p>

<h2>$example Example</h2>
<p>$description</p>
<p>Select language:</p>

<table>
__EOF__

  return $example_index_file;
}  # open_example_index


sub close_example_index {
  my ($example_index_file) = @_;

  print $example_index_file <<__EOF__;
</table>
</body>
</html>
__EOF__

  close($example_index_file);
}  # close_example_index
