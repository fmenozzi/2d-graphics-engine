/**
 *  Copyright 2015 Mike Reed
 *
 *  575 Introduction to Graphics
 */

#include "image.h"
#include "GBitmap.h"
#include "GCanvas.h"
#include "GMath.h"
#include <string>
#include <sys/stat.h>
#include <unistd.h>

static bool is_dir(const char path[]) {
    struct stat status;
    return !stat(path, &status) && (status.st_mode & S_IFDIR);
}

static bool mk_dir(const char path[]) {
    if (is_dir(path)) {
        return true;
    }
    if (!access(path, F_OK)) {
        fprintf(stderr, "%s exists but is not a directory\n", path);
        return false;
    }
    if (mkdir(path, 0777)) {
        fprintf(stderr, "error creating dir %s\n", path);
        return false;
    }
    return true;
}

static int pixel_diff(GPixel p0, GPixel p1) {
    int da = abs(GPixel_GetA(p0) - GPixel_GetA(p1));
    int dr = abs(GPixel_GetR(p0) - GPixel_GetR(p1));
    int dg = abs(GPixel_GetG(p0) - GPixel_GetG(p1));
    int db = abs(GPixel_GetB(p0) - GPixel_GetB(p1));
    return std::max(da, std::max(dr, std::max(dg, db)));
}

static double compare(const GBitmap& a, const GBitmap& b, int tolerance, bool verbose) {
    GASSERT(a.width() == b.width(),   "compare(): bitmap widths do not match");
    GASSERT(a.height() == b.height(), "compare(): bitmap heights do not match");

    const int total = a.width() * a.height() * 255;

    const GPixel* rowA = a.pixels();
    const GPixel* rowB = b.pixels();

    int total_diff = 0;
    int max_diff = 0;

    for (int y = 0; y < a.height(); ++y) {
        for (int x = 0; x < a.width(); ++x) {
            int diff = pixel_diff(rowA[x], rowB[x]) - tolerance;
            if (diff > 0) {
                total_diff += diff;
                max_diff = std::max(max_diff, diff);
            }
        }
        rowA = (const GPixel*)((const char*)rowA + a.rowBytes());
        rowB = (const GPixel*)((const char*)rowB + b.rowBytes());
    }
    
    double score = 1.0 * (total - total_diff) / total;
    if (verbose) {
        printf("    - score %d, max_diff %d\n", (int)(score * 100), max_diff);
    }
    return score;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

static void setup_bitmap(GBitmap* bitmap, int w, int h) {
    bitmap->fWidth = w;
    bitmap->fHeight = h;
    bitmap->fRowBytes = w * sizeof(GPixel);
    bitmap->fPixels = (GPixel*)calloc(bitmap->rowBytes(), bitmap->height());
}

static GPixel* get_addr(const GBitmap& bm, int x, int y) {
    GASSERT(x >= 0 && x < bm.width(),  "get_addr(): x is not in range");
    GASSERT(y >= 0 && y < bm.height(), "get_addr(): y is not in range");
    return bm.pixels() + x + y * (bm.rowBytes() >> 2);
}

static void handle_proc(const CS575DrawRec& rec, const char path[], GBitmap* bitmap) {
    setup_bitmap(bitmap, rec.fWidth, rec.fHeight);

    GCanvas* canvas = GCanvas::Create(*bitmap);
    if (NULL == canvas) {
        fprintf(stderr, "failed to create canvas for [%d %d] %s\n",
                rec.fWidth, rec.fHeight, rec.fName);
        return;
    }

    rec.fDraw(canvas);

    if (!bitmap->writeToFile(path)) {
        fprintf(stderr, "failed to write %s\n", path);
    }

    delete canvas;
}

static bool is_arg(const char arg[], const char name[]) {
    std::string str("--");
    str += name;
    if (!strcmp(arg, str.c_str())) {
        return true;
    }

    char shortVers[3];
    shortVers[0] = '-';
    shortVers[1] = name[0];
    shortVers[2] = 0;
    return !strcmp(arg, shortVers);
}

static void add_image(FILE* f, const char path[], const char name[], const char suffix[],
                      const GBitmap& bm) {
    std::string str(name);
    str += "__";
    str += suffix;
    str += ".png";
    fprintf(f, "<a href=\"%s\"><img src=\"%s\" /></a>\n", str.c_str(), str.c_str());

    std::string full(path);
    full += "/";
    full += str;
    bm.writeToFile(full.c_str());
}

static void add_diff_to_file(FILE* f, const GBitmap& test, const GBitmap& orig, const char path[],
                             const char name[]) {
    const int w = test.width();
    const int h = test.height();
    GBitmap diff0, diff1;
    setup_bitmap(&diff0, w, h);
    setup_bitmap(&diff1, w, h);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            int diff = pixel_diff(*get_addr(test, x, y), *get_addr(orig, x, y));
            *get_addr(diff0, x, y) = GPixel_PackARGB(0xFF, diff, diff, diff);
            if (diff > 0) {
                diff = 0xFF;
            }
            *get_addr(diff1, x, y) = GPixel_PackARGB(0xFF, diff, diff, diff);
        }
    }

    fprintf(f, "%s<br/>\n", name);
    add_image(f, path, name, "test", test); fprintf(f, "&nbsp;&nbsp;");
    add_image(f, path, name, "orig", orig); fprintf(f, "&nbsp;&nbsp;");
    add_image(f, path, name, "dif0", diff0); fprintf(f, "&nbsp;&nbsp;");
    add_image(f, path, name, "dif1", diff1); fprintf(f, "<br><br>\n");
}

int main(int argc, char** argv) {
    bool verbose = false;
    std::string root;
    const char* match = NULL;
    const char* expected = NULL;
    const char* diffDir = NULL;
    const char* report = NULL;
    const char* author = NULL;
    FILE* reportFile = NULL;
    FILE* diffFile = NULL;
    int tolerance = 0;

    for (int i = 1; i < argc; ++i) {
        if (is_arg(argv[i], "report") && i+2 < argc) {
            report = argv[++i];
            author = argv[++i];
            reportFile = fopen(report, "a");
            if (!reportFile) {
                printf("----- can't open %s for author %s\n", report, author);
                return -1;
            }
        } else if (is_arg(argv[i], "verbose")) {
            verbose = true;
        } else if (is_arg(argv[i], "write") && i+1 < argc) {
            root = argv[++i];
        } else if (is_arg(argv[i], "match") && i+1 < argc) {
            match = argv[++i];
        } else if (is_arg(argv[i], "expected") && i+1 < argc) {
            expected = argv[++i];
        } else if (is_arg(argv[i], "tolerance") && i+1 < argc) {
            tolerance = atoi(argv[++i]);
            GASSERT(tolerance >= 0, "main(): tolerance should be non-negative");
        } else if (is_arg(argv[i], "diff") && i+1 < argc) {
            diffDir = argv[++i];
            std::string path(diffDir);
            path += "/index.html";
            diffFile = fopen(path.c_str(), "w");
            if (!diffFile) {
                printf("------- failed to create %s\n", path.c_str());
            } else {
                fprintf(diffFile, "<h3>Test Orig Diff DIFF</h3>\n");
            }
        }
    }

    if (root.size() > 0 && root[root.size() - 1] != '/') {
        root += "/";
        if (!mk_dir(root.c_str())) {
            return -1;
        }
    }

    if (verbose) {
        printf("-- tolerance = %d\n", tolerance);
    }
    
    double total_correct = 0;
    int counter = 0;
    for (int i = 0; gDrawRecs[i].fDraw; ++i) {
        // weight the counter
        counter += gDrawRecs[i].fPANumber;

        std::string path(root);
        path += gDrawRecs[i].fName;
        path += ".png";

        if (match && !strstr(path.c_str(), match)) {
            continue;
        }
        if (verbose) {
            printf("image: %s\n", path.c_str());
        }
        
        GBitmap testBM;
        handle_proc(gDrawRecs[i], path.c_str(), &testBM);

        if (expected) {
            std::string exp_path(expected);
            exp_path += "/";
            exp_path += gDrawRecs[i].fName;
            exp_path += ".png";
            GBitmap expectedBM;
    
            if (!expectedBM.readFromFile(exp_path.c_str())) {
                printf("- failed to load <%s>\n", exp_path.c_str());
            } else {
                double correct = compare(testBM, expectedBM, tolerance, verbose);
                if (correct < 1 && diffFile != NULL) {
                    add_diff_to_file(diffFile, testBM, expectedBM, diffDir, gDrawRecs[i].fName);
                }
                
                // weight the score
                correct *= gDrawRecs[i].fPANumber;

                total_correct += correct;
            }
        }
        
        free(testBM.fPixels);
    }
    if (diffFile) {
        fclose(diffFile);
    }

    int image_score = GRoundToInt(total_correct * 100 / counter);
    if (expected) {
        printf("           image: %d\n", image_score);
    }
    if (reportFile) {
        fprintf(reportFile, "%s, image, %d\n", author, image_score);
    }
    return 0;
}
