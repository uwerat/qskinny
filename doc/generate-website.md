# Generating the website

## Prerequisites

You will need:

1. The `documentation-xml-website` branch of the Edelhirsch QSkinny repository
    at https://github.com/edelhirsch/qskinny/tree/documentation-xml-website .
1. A recent version of doxygen; The currently used version is 1.9.1 built from
    github sources. The `doxygen` binary needs to be in the $PATH.
1. A recent version of doxybook2 with some custom patches. The script
    `generate-website.sh` should download and build the right version, however
    the script might need some adaptation of paths.
    For reference, the required version can be found at
    https://github.com/edelhirsch/doxybook2/tree/jekyll .
1. A recent version of Jekyll (see https://jekyllrb.com/), which will generate
    the static html pages. This and other required packages can be installed via
    ```
    gem install jekyll:3.9.0
    gem install bundler:2.1.4
    ```
    There might be some packages missing from the list above; in this case the
    Gemfile in the qskinny-website repository might help.
1. Checkout the current website repository via
    `git clone git@github.com:qskinny/qskinny.github.io.git`

## Generating the website

Generating the static HTML sites is done with the `generate-website.sh` script
in the `qskinny/doc` directory. The script has some hardcoded paths and probably
needs some adaptation to run correctly.

It will do the following:

1. Generate HTML from doxygen. This step is needed because for some reason when
    generating XML from doxygen there are no images with dependency graphs.
    *Note*: This step is only executed if the `html` folder doesn't exist,
    because otherwise it would take too long.
1. Generate XML from doxygen. The generated XML is used with doxybook2 in the
    next step.
    *Note*: This step is only executed if the `xml` folder doesn't exist,
    because otherwise it would take too long.
1. Generate markdown from XML with doxybook2. This markdown will be used by
    Jekyll to either server the website content locally or generate static
    HTML from it, see below.

### Generating the website locally

When the command line switch `-local` is used with the `generate-website.sh`
script, it will generate the content to a local folder `doxybook-out`. This is
meant to be able to copy selected files to the website directory at
`~/dev/qskinny-website`.
Otherwise, the script will copy the content to the website repository for
uploading (again, paths are hardcoded as of now). So when generating content
for the first time, just run the script without any switches, which should
generate the website to `~/dev/qskinny-website`.

### Testing the website locally

After having generated the website as described above, go to
`~/dev/qskinny-website` and run `jekyll serve --livereload`. This should start
a browser at http://127.0.0.1:4000/, which will display the website.

### Generating the website publicly

When the command line switch `-publish` is used, the script will automatically
generate a new version of the homepage and publish it at
https://qskinny.github.io . This wil only work with the proper user rights of
course.
