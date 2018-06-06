#include <iostream>
#include <cmath>
#include <b2d/b2d.h>
#include <PDF/PDFNet.h>
#include <PDF/PDFDoc.h>
#include <PDF/Element.h>
#include <PDF/ElementReader.h>

std::string rootPath;
b2d::Context2D* ctx;
char m_buf[4000];
double line_width = 1.0;

b2d::Argb32 PdfRgb2B2DArgb32(pdftron::PDF::ColorPt& rgb)
{
    pdftron::UInt8 r,g,b;
    r = rgb.Get(0) * 255;
    g = rgb.Get(1) * 255;
    b = rgb.Get(2) * 255;
    pdftron::UInt32 x = 0xFF000000 + (r << 16) + (g << 8) + b;
    return b2d::Argb32(x);
}

b2d::Matrix2D PdfMtx2B2DMatx(const pdftron::Common::Matrix2D& mtx)
{
    return {mtx.m_a, mtx.m_b, mtx.m_c, mtx.m_d, mtx.m_h, mtx.m_v};
}

void parseGsChanges(pdftron::PDF::PDFDoc& doc, pdftron::PDF::Page& page, pdftron::PDF::ElementReader& reader, pdftron::PDF::Element& element)
{
    pdftron::PDF::GState gs = element.GetGState();
    for (pdftron::PDF::GSChangesIterator itr = reader.GetChangesIterator(); itr.HasNext(); itr.Next())
    {
        switch(itr.Current())
        {
            case pdftron::PDF::GState::e_transform:
                {
                    pdftron::Common::Matrix2D ctm = gs.GetTransform();
                    //b2d::Matrix2D mtx = PdfMtx2B2DMatx(ctm);
                    std::sprintf(m_buf, "%.2f %.2f %.2f %.2f %.2f %.2f cm\n", ctm.m_a, ctm.m_b, ctm.m_c, ctm.m_d, ctm.m_h, ctm.m_v);
                    std::cout << m_buf;
                }
                break;
            case pdftron::PDF::GState::e_rendering_intent:
                std::cout << "intent\n";
                break;
            case pdftron::PDF::GState::e_stroke_cs:
                std::cout << "CS\n";
                break;
            case pdftron::PDF::GState::e_stroke_color:
                std::cout << "COLOR\n";
                {
                    pdftron::PDF::ColorSpace stroke_cs = gs.GetStrokeColorSpace();
                    pdftron::PDF::ColorPt stroke_cp = gs.GetStrokeColor();
                    pdftron::PDF::ColorPt stroke_rgb_cp = stroke_cs.Convert2RGB(stroke_cp);
                    b2d::Argb32 stroke_color = PdfRgb2B2DArgb32(stroke_rgb_cp);
                    ctx->setStrokeStyle(stroke_color);
                }
                break;
            case pdftron::PDF::GState::e_fill_cs:
                std::cout << "cs\n";
                break;
            case pdftron::PDF::GState::e_fill_color:
                std::cout << "color\n";
                {
                    pdftron::PDF::ColorSpace fill_cs = gs.GetFillColorSpace();
                    pdftron::PDF::ColorPt fill_cp = gs.GetFillColor();
                    pdftron::PDF::ColorPt fill_rgb_cp = fill_cs.Convert2RGB(fill_cp);
                    b2d::Argb32 fill_color = PdfRgb2B2DArgb32(fill_rgb_cp);
                    ctx->setFillStyle(fill_color);
                }
                break;
            case pdftron::PDF::GState::e_line_width:
                {
                    line_width = gs.GetLineWidth();
                    std::sprintf(m_buf, "%.2f w\n", line_width);
                    std::cout << m_buf;
                }
                break;
            case pdftron::PDF::GState::e_line_cap:
                std::cout << "line cap\n";
                switch(gs.GetLineCap())
                {
                    case pdftron::PDF::GState::LineCap::e_butt_cap:
                        ctx->setStartCap(b2d::StrokeCap::kButt);
                        ctx->setEndCap(b2d::StrokeCap::kButt);
                        break;
                    case pdftron::PDF::GState::LineCap::e_round_cap:
                        ctx->setStartCap(b2d::StrokeCap::kRound);
                        ctx->setEndCap(b2d::StrokeCap::kRound);
                        break;
                    case pdftron::PDF::GState::LineCap::e_square_cap:
                        ctx->setStartCap(b2d::StrokeCap::kSquare);
                        ctx->setEndCap(b2d::StrokeCap::kSquare);
                        break;
                }
                break;
            case pdftron::PDF::GState::e_line_join:
                std::cout << "line join\n";
                switch(gs.GetLineJoin())
                {
                    case pdftron::PDF::GState::e_miter_join:
                        ctx->setLineCaps(b2d::StrokeJoin::kMiter);
                        break;
                        
                    case pdftron::PDF::GState::e_round_join:
                        ctx->setLineCaps(b2d::StrokeJoin::kRound);
                        break;
                    case pdftron::PDF::GState::e_bevel_join:
                        ctx->setLineCaps(b2d::StrokeJoin::kBevel);
                        break;
                }
                break;
            case pdftron::PDF::GState::e_flatness:
                std::cout << "flatness\n";
                break;
            case pdftron::PDF::GState::e_miter_limit:
                std::cout << "miter limit\n";
                break;
            case pdftron::PDF::GState::e_dash_pattern:
                std::cout << "dash\n";
                break;
            case pdftron::PDF::GState::e_char_spacing:
                std::cout << "char spacing\n";
                break;
            case pdftron::PDF::GState::e_word_spacing:
                std::cout << "word spacing\n";
                break;
            case pdftron::PDF::GState::e_horizontal_scale:
                std::cout << "horiz scale\n";
                break;
            case pdftron::PDF::GState::e_leading:
                std::cout << "leading\n";
                break;
            case pdftron::PDF::GState::e_font:
                std::cout << "font\n";
                break;
            case pdftron::PDF::GState::e_font_size:
                std::cout << "text font size\n";
                break;
            case pdftron::PDF::GState::e_text_render_mode:
                std::cout << "text rm\n";
                break;
            case pdftron::PDF::GState::e_text_rise:
                std::cout << "text rise\n";
                break;
            case pdftron::PDF::GState::e_text_knockout:
                std::cout << "text knockout\n";
                break;
            case pdftron::PDF::GState::e_text_pos_offset:
                std::cout << "text pos offset\n";
                break;
            case pdftron::PDF::GState::e_blend_mode:
                std::cout << "BM\n";
                break;
            case pdftron::PDF::GState::e_opacity_fill:
                std::cout << "ca\n";
                break;
            case pdftron::PDF::GState::e_opacity_stroke:
                std::cout << "CA\n";
                break;
            case pdftron::PDF::GState::e_alpha_is_shape:
                std::cout << "alpha is shape\n";
                break;
            case pdftron::PDF::GState::e_soft_mask:
                std::cout << "soft mask\n";
                break;
            case pdftron::PDF::GState::e_smoothnes:
                std::cout << "smoothness\n";
                break;
            case pdftron::PDF::GState::e_auto_stoke_adjust:
                std::cout << "\n";
                break;
            case pdftron::PDF::GState::e_stroke_overprint:
                std::cout << "OP\n";
                break;
            case pdftron::PDF::GState::e_fill_overprint:
                std::cout << "op\n";
                break;
            case pdftron::PDF::GState::e_overprint_mode:
                std::cout << "op mode\n";
                break;
            case pdftron::PDF::GState::e_transfer_funct:
                std::cout << "transfer funct\n";
                break;
            case pdftron::PDF::GState::e_BG_funct:
                std::cout << "BG funct\n";
                break;
            case pdftron::PDF::GState::e_UCR_funct:
                std::cout << "UCR funct\n";
                break;
            case pdftron::PDF::GState::e_halftone:
                std::cout << "halftone\n";
                break;
            case pdftron::PDF::GState::e_null:
                std::cout << "null\n";
                break;
        }

    }
    reader.ClearChangeList();
}

void renderPath(pdftron::PDF::PDFDoc& doc, pdftron::PDF::Page& page, pdftron::PDF::ElementReader& reader, pdftron::PDF::Element& element)
{
    pdftron::Common::Matrix2D ctm = element.GetCTM();
    b2d::Matrix2D mtx = PdfMtx2B2DMatx(ctm);
    const double avg_scale = mtx.averageScaling();
    ctx->setStrokeWidth(line_width * avg_scale);
    ctm = pdftron::Common::Matrix2D(1,0,0,-1,0,792) * ctm;
    
    if (element.IsClippingPath())
    {
        std::cout << "This is a clipping path" << std::endl;
    }
    
    b2d::Path2D path;
    
    ctx->setCompOp(b2d::CompOp::kSrcOver);
    
    pdftron::PDF::PathData d = element.GetPathData();
    
    const pdftron::UChar* opr =  &d.GetOperators().front();
    const pdftron::UChar *opr_itr = opr, *opr_end = opr + d.GetOperators().size();
    const double* data =  &d.GetPoints().front();
    const double *data_itr = data /*, *data_end = data + d.GetPoints().size()*/;
    
    double x1, y1, x2, y2, x3, y3;

    for (; opr_itr<opr_end; ++opr_itr)
    {
        switch(*opr_itr)
        {
            case pdftron::PDF::PathData::e_moveto:
                x1 = *data_itr; ++data_itr;
                y1 = *data_itr; ++data_itr;
                std::sprintf(m_buf, "M%.0f %.0f", x1, y1);
                //std::cout << m_buf;
                ctm.Mult(x1, y1);
                path.moveTo(x1, y1);
                break;
            case pdftron::PDF::PathData::e_lineto:
                x1 = *data_itr; ++data_itr;
                y1 = *data_itr; ++data_itr;
                std::sprintf(m_buf, " L%.0f %.0f", x1, y1);
                //std::cout << m_buf;
                ctm.Mult(x1, y1);
                path.lineTo(x1, y1);
                break;
                
            case pdftron::PDF::PathData::e_cubicto:
                x1 = *data_itr; ++data_itr;
                y1 = *data_itr; ++data_itr;
                x2 = *data_itr; ++data_itr;
                y2 = *data_itr; ++data_itr;
                x3 = *data_itr; ++data_itr;
                y3 = *data_itr; ++data_itr;
                std::sprintf(m_buf, " C%.0f %.0f %.0f %.0f %.0f %.0f", x1, y1, x2, y2, x3, y3);
                //std::cout << m_buf;
                ctm.Mult(x1, y1);
                ctm.Mult(x2, y2);
                ctm.Mult(x3, y3);
                path.cubicTo(x1, y1, x2, y2, x3, y3);
                break;
                
            case pdftron::PDF::PathData::e_rect:
                {
                    x1 = *data_itr; ++data_itr;
                    y1 = *data_itr; ++data_itr;
                    double w = *data_itr; ++data_itr;
                    double h = *data_itr; ++data_itr;
                    x2 = x1 + w;
                    y2 = y1;
                    x3 = x2;
                    y3 = y1 + h;
                    double x4 = x1;
                    double y4 = y3;
                    std::sprintf(m_buf, "M%.0f %.0f L%.0f %.0f L%.0f %.0f L%.0f %.0f Z",
                            x1, y1, x2, y2, x3, y3, x4, y4);
                    //std::cout << m_buf;
                    ctm.Mult(x1, y1);
                    ctm.Mult(x2, y2);
                    ctm.Mult(x3, y3);
                    ctm.Mult(x4, y4);
                    path.moveTo(x1,y1);
                    path.lineTo(x2,y2);
                    path.lineTo(x3,y3);
                    path.lineTo(x4,y4);
                    path.close();
                }
                break;
                
            case pdftron::PDF::PathData::e_closepath:
                //std::cout << " Close Path" << std::endl;
                path.close();
                break;
            default:
                assert(false);
                break;
        }
    }
    
    if(element.IsFilled()) ctx->fillPath(path);
    if(element.IsStroked()) ctx->strokePath(path);
}

void renderElements(pdftron::PDF::PDFDoc& doc, pdftron::PDF::Page& page, pdftron::PDF::ElementReader& reader)
{
    for (pdftron::PDF::Element element = reader.Next(); element; element = reader.Next())
    {
        parseGsChanges(doc, page, reader, element);
        switch (element.GetType())
        {
            case pdftron::PDF::Element::e_null:
                std::cout << "null\n";
                break;
            case pdftron::PDF::Element::e_path:
                std::cout << "path\n";
                renderPath(doc, page, reader, element);
                break;
            case pdftron::PDF::Element::e_text_begin:
                std::cout << "BT\n";
                break;
            case pdftron::PDF::Element::e_text:
                std::cout << "text\n";
                break;
            case pdftron::PDF::Element::e_text_new_line:
                std::cout << "NL\n";
                break;
            case pdftron::PDF::Element::e_text_end:
                std::cout << "ET\n";
                break;
            case pdftron::PDF::Element::e_image:
                std::cout << "image\n";
                break;
            case pdftron::PDF::Element::e_inline_image:
                std::cout << "inline image\n";
                break;
            case pdftron::PDF::Element::e_shading:
                std::cout << "shading\n";
                break;
            case pdftron::PDF::Element::e_form:
                std::cout << "do\n";
                break;
            case pdftron::PDF::Element::e_group_begin:
                std::cout << "q\n";
                break;
            case pdftron::PDF::Element::e_group_end:
                std::cout << "Q\n";
                break;
            case pdftron::PDF::Element::e_marked_content_begin:
                std::cout << "BMC\n";
                break;
            case pdftron::PDF::Element::e_marked_content_end:
                std::cout << "EMC\n";
                break;
            case pdftron::PDF::Element::e_marked_content_point:
                std::cout << "MP\n";
                break;
        }
    }

}

void renderPage(pdftron::PDF::PDFDoc& doc, pdftron::PDF::Page& page)
{
    b2d::Image img(612, 792, b2d::PixelFormat::kPRGB32);
    b2d::Context2D b2dctx(img);
    ctx = &b2dctx;
    
    ctx->setCompOp(b2d::CompOp::kSrc);
    ctx->setFillStyle(b2d::Argb32(0xFFFFFFFF));
    ctx->fillAll();
    
    ctx->setCompOp(b2d::CompOp::kSrcOver);
    ctx->setFillStyle(b2d::Argb32(0xFF000000));
    
    pdftron::PDF::ElementReader reader;
    reader.Begin(page);
    renderElements(doc, page, reader);
    reader.End();
    
    ctx->end();
    
    b2d::ImageCodec codec = b2d::ImageCodec::codecByName(b2d::ImageCodec::builtinCodecs(), "BMP");
    b2d::ImageUtils::writeImageToFile("b2d-tiger_07.bmp", codec, img);
}

void renderPdf(const std::string& pdf_path)
{
    try
    {
        pdftron::PDF::PDFDoc doc(pdf_path);
        if(doc.HasRepairedXRef())
        {
            std::cout << "PDF has malformed XRef table and has been repaired\n";
        }
        if(!doc.InitSecurityHandler())
        {
            std::cout << "PDF is password protected. Cannot read\n";
            return;
        }
        for (pdftron::PDF::PageIterator itr = doc.GetPageIterator(); itr.HasNext(); itr.Next())
        {
            try {
                renderPage(doc, itr.Current());
            } catch (std::exception& e) {
                std::cerr << "Error rendering page " << itr.Current().GetIndex() << "\n" << e.what() << std::endl;
            }
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char** argv)
{
    pdftron::PDFNet::Initialize();
    std::cout << "PDFNet " << pdftron::PDFNet::GetVersion() << std::endl;
    
    if(argc < 3) return 0;
    
    rootPath = argv[2];
    
    renderPdf(argv[1]);
    
    return 0;
}
