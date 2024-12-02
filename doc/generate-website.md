# Generating the website

## Prerequisites

You will need:

1. A recent version of doxygen; The currently used version is 1.9.8.
    The `doxygen` binary needs to be in the $PATH.
1. The `m4` command, which can be installed with `sudo apt install m4`.
1. A recent version of Jekyll (see https://jekyllrb.com/), which will generate
    the static html pages. The currently used jekyll version is 4.2.2.
    Also a recent version of bundler is required; this can be installed with:
    ```
    gem install jekyll:4.2.2
    gem install bundler:2.1.4
    ```
1. Checkout the repo to generate the website via
    `git clone git@github.com:peter-ha/qskinny-website.git`
1. Checkout the live website repository via
    `git clone git@github.com:qskinny/qskinny.github.io.git`


## Generating the website

### Generating the API documentation with doxygen
```
cd ~/dev/qskinny/doc
export PATH=.:$PATH
doxygen
```
This will generate the documentation into the `api` folder.

### Testing and building the website locally

First copy the generated files from above to the website repo:

```
cp -r api ~/dev/qskinny-website/docs/
```

Then test the website locally:

```
cd ~/dev/qskinny-website
bundle exec jekyll serve --livereload
```

Then direct your browser to `http://127.0.0.1:4000/`.

If all looks good, generate the static HTML website:

```
bundle exec jekyll build
```

### Publishing the website

Just copy over the generated HTML files to the public website repo and push a
new version of the homepage:

```
cp -r _site/* ~/dev/qskinny.github.io/
cd ~/dev/qskinny.github.io/
git commit -a -m "new version"  # you might want to add new files
gith push
```

That's it, the new website is now published at https://qskinny.github.io/ .
